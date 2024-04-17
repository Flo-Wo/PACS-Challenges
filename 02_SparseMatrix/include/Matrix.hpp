#ifndef MY_SPARSE_MATRIX_HPP
#define MY_SPARSE_MATRIX_HPP
#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "Utilities.hpp"

// TODO: DOCUMENTATION
// Question: is the map in the correct ordering given (assumption), or do we
// have to change this by our own?
namespace algebra {
template <class T, StorageOrder Store = StorageOrder::row>
class Matrix {
 public:
  using EntryValuesMap = std::map<
      std::array<std::size_t, 2>, T,
      std::conditional_t<Store == StorageOrder::row, RowOrderComparator<T>,
                         ColOrderComparator<T>>>;

  Matrix(EntryValuesMap& entry_value_map)
      : _is_compressed(false),
        _entry_value_map(entry_value_map),
        _vec1(),
        _vec2(),
        _values(){};

  Matrix(std::vector<std::size_t> vec1, std::vector<std::size_t> vec2,
         std::vector<T> values)
      : _is_compressed(true),
        _entry_value_map(),
        _vec1(vec1),
        _vec2(vec2),
        _values(values){};

  template <NormOrder Norm>
  T norm() const {
#ifdef DEBUG
    std::cout << "Calling Norm with " << Norm << "\n";
#endif
    // FROB norm is the easiest case
    if constexpr (Norm == NormOrder::frob) {
      if (!_is_compressed)
        return _frob_norm_uncompressed();
      else
        return _frob_norm_compressed();
    }

    if (!_is_compressed) return _norm_uncompressed<Norm>();
    // col compression
    if constexpr (Store == StorageOrder::col) {
      if constexpr (Norm == NormOrder::one) {
        return _one_norm_compressed_col();
      }
      return _max_norm_compressed_col();
    }
    // row compression
    if constexpr (Norm == NormOrder::one) {
      return _one_norm_compressed_row();
    }
    return _max_norm_compressed_row();
  };

  T& operator()(std::size_t row, std::size_t col) {
#ifdef DEBUG
    std::cout << "Non-const operator() is called.\n";
#endif
    if (!_is_compressed) {
      std::array<std::size_t, 2> find{row, col};
      // either add or override, both is fine
      return _entry_value_map[find];
    }
    if constexpr (Store == StorageOrder::row) {
      // only existing values can be added
      return this->_find_compressed_element_row(row, col);
    } else {
      // only existing values can be added
      return this->_find_compressed_element_col(row, col);
    }
  };
  T operator()(std::size_t row, std::size_t col) const {
#ifdef DEBUG
    std::cout << "Const operator() is called.\n";
#endif
    if (!_is_compressed) {
      return this->_find_uncompressed_element(row, col);
    }
    // compressed case, check for row/col format
    if constexpr (Store == StorageOrder::row) {
      return this->_find_compressed_element_row(row, col);
    }
    return this->_find_compressed_element_col(row, col);
  };

  friend std::ostream& operator<<(std::ostream& os,
                                  const Matrix<T, Store> matrix) {
    if (!matrix.is_compressed()) {
      for (const auto& [k, v] : matrix._entry_value_map) {
        os << "[" << k[0] << ", " << k[1] << "] = " << v << "\n";
      }
      return os;
    }
    os << "Compression format: " << Store << "\n\n";
    os << "vec1 = \n";
    for (const auto& el : matrix._vec1) {
      os << el << ", ";
    }
    os << "\nvec2 = \n";
    for (const auto& el : matrix._vec2) {
      os << el << ", ";
    }
    os << "\nvalues = \n";
    for (const auto& el : matrix._values) {
      os << el << ", ";
    }
    os << "\n";
    return os;
  }

  std::vector<T> operator*(std::vector<T> vec) const {
    if (!_is_compressed) {
#ifdef DEBUG
      std::cout << "Calling matrix*vector without compression\n";
#endif
      return _matrix_vector_uncompressed(vec);
    }
    if constexpr (Store == StorageOrder::row) {
#ifdef DEBUG
      std::cout << "Calling matrix*vector for row compression\n";
#endif
      return _matrix_vector_row(vec);
    }
#ifdef DEBUG
    std::cout << "Calling matrix*vector for column compression\n";
#endif
    return _matrix_vector_col(vec);
  };

  void compress() {
    if constexpr (Store == StorageOrder::row) {
      _compress_row();
    } else {
      _compress_col();
    };
  }
  void uncompress() {
    if constexpr (Store == StorageOrder::row) {
      _uncompress_row();
    } else {
      _uncompress_col();
    };
  }

  bool is_compressed() const { return _is_compressed; };

 private:
  template <NormOrder Norm>
  T _norm_uncompressed() const {
    // ONE NORM
    if constexpr (Norm == NormOrder::one) {
      return _one_norm_uncompressed();
    }
    return _max_norm_uncompressed();
  };

  T _frob_norm_uncompressed() const {
#ifdef DEBUG
    std::cout << "Frobenius Norm uncompressed.\n";
#endif
    T res = 0;
    for (const auto& [k, v] : _entry_value_map) res += std::norm(v);
    return std::sqrt(res);
  };
  T _frob_norm_compressed() const {
#ifdef DEBUG
    std::cout << "Frobenius Norm compressed.\n";
#endif
    T res = 0;
    for (const auto& val : _values) res += std::norm(val);
    return std::sqrt(res);
  }

  // max of the sum_abs(cols)
  T _one_norm_uncompressed() const {
#ifdef DEBUG
    std::cout << "One-Norm uncompressed.\n";
#endif
    std::size_t num_cols = 0;
    if constexpr (Store == StorageOrder::row) {
      for (const auto& [k, v] : _entry_value_map) {
        num_cols = std::max(num_cols, k[1] + 1);
      }
    } else {
      num_cols = _entry_value_map.rbegin()->first[1] + 1;
    }

#ifdef DEBUG
    std::cout << "num_cols = " << num_cols << "\n";
#endif
    std::vector<T> sum_abs_per_col(num_cols, 0.0);
    for (const auto& [k, v] : _entry_value_map) {
      sum_abs_per_col[k[1]] += std::abs(v);
    }
    return *max_element(std::begin(sum_abs_per_col), std::end(sum_abs_per_col));
  };

  // max of the sum_abs(rows)
  T _max_norm_uncompressed() const {
#ifdef DEBUG
    std::cout << "One-Norm compressed.\n";
#endif
    std::size_t num_rows = 0;
    if constexpr (Store == StorageOrder::row) {
      num_rows = _entry_value_map.rbegin()->first[0] + 1;
    } else {
      for (const auto& [k, v] : _entry_value_map) {
        num_rows = std::max(num_rows, k[0] + 1);
      }
    }
    std::vector<T> sum_abs_per_row(num_rows, 0.0);
    for (const auto& [k, v] : _entry_value_map) {
      sum_abs_per_row[k[0]] += std::abs(v);
    }
    return *max_element(std::begin(sum_abs_per_row), std::end(sum_abs_per_row));
  };

  T _find_uncompressed_element(std::size_t row, std::size_t col) const {
    std::array<std::size_t, 2> to_find{row, col};
    if (auto search = _entry_value_map.find(to_find);
        search != _entry_value_map.end()) {
#ifdef DEBUG
      std::cout << "Found the element: " << search->first[0] << ", "
                << search->first[1] << "\n";
#endif
      return search->second;
    }
    return 0;
  }
  std::vector<T> _matrix_vector_uncompressed(std::vector<T> vec) const {
    std::cout << "here\n";
    std::vector<T> res;
#ifdef DEBUG
    std::cout << "vec.size()" << vec.size() << "\n";
    std::cout << "res.size()" << res.size() << "\n";
    std::cout << _entry_value_map.rbegin()->first[0] + 1 << "\n";
#endif
    res.resize(_entry_value_map.rbegin()->first[0] + 1, 0);
    for (const auto& [k, v] : _entry_value_map) {
#ifdef DEBUG
      std::cout << "k[0] " << k[0] << ", k[1] " << k[1] << ": " << v << "\n";
      std::cout << "vec[k[1]] = " << vec[k[1]] << "\n";
      std::cout << "res[k[0]] = " << res[k[0]] << "\n";
#endif
      res[k[0]] += (vec[k[1]] * v);
    }
    return res;
  }

  // specialization to decide via const-expr
  void _compress_row();
  void _uncompress_row();
  const T _find_compressed_element_row(std::size_t row, std::size_t col) const;
  T& _find_compressed_element_row(std::size_t row, std::size_t col);
  std::vector<T> _matrix_vector_row(std::vector<T>) const;
  T _one_norm_compressed_row() const;
  T _max_norm_compressed_row() const;

  void _compress_col();
  void _uncompress_col();
  const T _find_compressed_element_col(std::size_t row, std::size_t col) const;
  T& _find_compressed_element_col(std::size_t row, std::size_t col);
  std::vector<T> _matrix_vector_col(std::vector<T>) const;
  T _one_norm_compressed_col() const;
  T _max_norm_compressed_col() const;

  // class attributes
  bool _is_compressed;
  EntryValuesMap& _entry_value_map;

  // internal representations of the values for the compressed formats
  std::vector<std::size_t> _vec1;
  std::vector<std::size_t> _vec2;
  std::vector<T> _values;
};

// ======= ROW MAJOR OPERATIONS =======
// specialization for row-major, i.e. the default case
// convert internal mapping to compressed sparse row (CSR) format
#include "Matrix_row_impl.hpp"
// ======= COL MAJOR OPERATIONS =======
#include "Matrix_col_impl.hpp"
}  // namespace algebra
#endif
