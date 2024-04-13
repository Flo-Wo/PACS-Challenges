#ifndef MY_SPARSE_MATRIX_HPP
#define MY_SPARSE_MATRIX_HPP
#include <algorithm>  // TODO: remove?
#include <array>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "StorageOrder.hpp"

// TODO: DOCUMENTATION
// Question: is the map in the correct ordering given (assumption), or do we
// have to change this by our own?
namespace algebra {
template <class T, StorageOrder Store = StorageOrder::row>
class Matrix {
 public:
  // TODO: change this based on the Store template -> different comparison
  // operator
  using EntryValuesMap = std::map<
      std::array<std::size_t, 2>, T,
      std::conditional_t<Store == StorageOrder::row, RowOrderComparator<T>,
                         ColOrderComparator<T>>>;

  // TODO: change ordering if we want to be column-based
  Matrix(EntryValuesMap& entry_value_map)
      : _is_compressed(false),
        _entry_value_map(entry_value_map),
        _vec1(),
        _vec2(),
        _values(){};
  // Matrix(std::size_t size){};

  // TODO: leave matrix in uncompressed state
  void resize(std::size_t new_size){};

  T& operator()(std::size_t row, std::size_t col) {
#ifdef DEBUG
    std::cout << "Non-const operator() is called.\n";
#endif
    if constexpr (Store == StorageOrder::row) {
      if (!_is_compressed) {
        std::array<std::size_t, 2> find{row, col};
        // either add or override, both is fine
        return _entry_value_map[find];
      }
      // only existing values can be added
      return this->_find_compressed_element_row(row, col);
    } else {
      if (!_is_compressed) {
        std::array<std::size_t, 2> find{row, col};
        // either add or override, both is fine
        return _entry_value_map[find];
      }
      // only existing values can be added
      return this->_find_compressed_element_col(row, col);
    }
  };

  const T& operator()(std::size_t row, std::size_t col) const {
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
  T _find_uncompressed_element(std::size_t row, std::size_t col) const {
    std::array<std::size_t, 2> to_find{row, col};
    if (auto search = _entry_value_map.find(to_find);
        search != _entry_value_map.end()) {
#ifdef DEBUG
      std::cout << "Found the element: " << search->first << "\n";
#endif

      return search->second;
    }
    return 0;
  }
  std::vector<T> _matrix_vector_uncompressed(std::vector<T> vec) const {
    std::vector<T> res;
    res.resize(_entry_value_map.rbegin()->first[0] + 1, 0);
#ifdef DEBUG
    std::cout << "vec1.size()" << _vec1.size() << "\n";
    std::cout << "vec.size()" << vec.size() << "\n";
    std::cout << "res.size()" << res.size() << "\n";
#endif
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
  const T& _find_compressed_element_row(std::size_t row, std::size_t col) const;
  T& _find_compressed_element_row(std::size_t row, std::size_t col);
  std::vector<T> _matrix_vector_row(std::vector<T>) const;

  void _compress_col();
  void _uncompress_col();
  const T& _find_compressed_element_col(std::size_t row, std::size_t col) const;
  T& _find_compressed_element_col(std::size_t row, std::size_t col);
  std::vector<T> _matrix_vector_col(std::vector<T>) const;

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
template <class T, StorageOrder Store>
void Matrix<T, Store>::_compress_row() {
#ifdef DEBUG
  std::cout << "Using ROW-MAJOR compression to CSR.\n";
#endif
  // vec1 of length #rows + 1 -> row indices
  // vec2 of length #non-zero-elements -> column index
  // _values: length #non-zero-elements -> actual values

  // #rows = highest row-number + 2
  std::size_t num_rows = _entry_value_map.rbegin()->first[0] + 2;
  _vec1.resize(num_rows, 0);

  // number of non-zeros are simply the number of map entries
  std::size_t num_non_zeros = _entry_value_map.size();
  _vec2.resize(num_non_zeros);
  _values.resize(num_non_zeros);

#ifdef DEBUG
  std::cout << "num_rows = " << num_rows << "\n";
  std::cout << "vec2.size() = " << _vec2.size() << "\n";
  std::cout << "values.size() = " << _values.size() << "\n";
#endif

  std::size_t num_non_zero = 0;
  // idea: not use conditional jumps
  for (const auto& [k, v] : _entry_value_map) {
    _vec2[num_non_zero] = k[1];  // add the column index
    _values[num_non_zero] = v;   // add the value
    // we just update the count of non-zeros at the curr. row-idx
    // note that the row-idx is automatically incremented
    _vec1[k[0] + 1] = ++num_non_zero;
  }

  // save memory and set flags
  _is_compressed = true;
  _entry_value_map.clear();
}

template <class T, StorageOrder Storage>
void Matrix<T, Storage>::_uncompress_row() {
#ifdef DEBUG
  std::cout << "Using ROW-MAJOR uncompression\n";
#endif
  // vec1 of length #rows + 1 -> row indices
  // vec2 of length #non-zero-elements -> column index
  // _values: length #non-zero-elements -> actual values
  std::size_t num_rows = _vec1.size() - 1;
  for (std::size_t row_idx = 0; row_idx < num_rows; ++row_idx) {
    for (std::size_t col_idx = _vec1[row_idx]; col_idx < _vec1[row_idx + 1];
         ++col_idx) {
      // we get the col number and the value accordingly
      _entry_value_map[std::array<std::size_t, 2>{row_idx, _vec2[col_idx]}] =
          _values[col_idx];
    }
  }
  // save memory and set flags
  _is_compressed = false;
  _vec1.clear();
  _vec2.clear();
  _values.clear();
}

template <class T, StorageOrder Store>
const T& Matrix<T, Store>::_find_compressed_element_row(std::size_t row,
                                                        std::size_t col) const {
#ifdef DEBUG
  std::cout << "Using ROW-MAJOR _find_compressed_element() const version.\n ";
#endif
  for (std::size_t col_idx = _vec1[row]; col_idx < _vec1[row + 1]; ++col_idx) {
    if (_vec2[col_idx] == col) {
#ifdef DEBUG
      std::cout << "Found element.";
#endif
      return _values[col_idx];
    }
  }
  return 0;
}

template <class T, StorageOrder Store>
T& Matrix<T, Store>::_find_compressed_element_row(std::size_t row,
                                                  std::size_t col) {
#ifdef DEBUG
  std::cout
      << "Using ROW-MAJOR _find_compressed_element() non-const version.\n";
#endif
  for (std::size_t col_idx = _vec1[row]; col_idx < _vec1[row + 1]; ++col_idx) {
    if (_vec2[col_idx] == col) {
#ifdef DEBUG
      std::cout << "Found element.\n";
#endif
      return _values[col_idx];
    }
  }
  throw std::invalid_argument(
      "Trying to modify a zero-element in compressed format. Uncompress first");
}

template <class T, StorageOrder Store>
std::vector<T> Matrix<T, Store>::_matrix_vector_row(std::vector<T> vec) const {
  // iterate through the rows, then the elements
  std::vector<T> res;
  res.resize(_vec1.size() - 1, 0);

  for (std::size_t row_idx = 0; row_idx < _vec1.size() - 1; ++row_idx) {
    // get the columns, according to this row
    for (std::size_t col_idx = _vec1[row_idx]; col_idx < _vec1[row_idx + 1];
         ++col_idx) {
      res[row_idx] += (vec[_vec2[col_idx]] * _values[col_idx]);
    }
  }
  return res;
}

// ======= COL MAJOR OPERATIONS =======
// default case
template <class T, StorageOrder Store>
void Matrix<T, Store>::_compress_col() {
#ifdef DEBUG
  std::cout << "Using COL-MAJOR compression to CSC.\n";
#endif
  // NOTE(Assumption): we assume the dict is orderd in (second,first) way
  // so (1,2) is after (2,1) in the ordering

  // vec1 of length #cols + 1 -> col indices
  // vec2 of length #non-zero-elements -> row index
  // _values: length #non-zero-elements -> actual values

  // #cols = highest col-number + 2
  std::size_t num_cols = _entry_value_map.rbegin()->first[1] + 2;
  _vec1.resize(num_cols, 0);

  // number of non-zeros are simply the number of map entries
  std::size_t num_non_zeros = _entry_value_map.size();
  _vec2.resize(num_non_zeros);
  _values.resize(num_non_zeros);

#ifdef DEBUG
  std::cout << "num_rows = " << num_cols << "\n";
  std::cout << "vec2.size() = " << _vec2.size() << "\n";
  std::cout << "values.size() = " << _values.size() << "\n";
#endif

  std::size_t num_non_zero = 0;
  // idea: not use conditional jumps
  for (const auto& [k, v] : _entry_value_map) {
    _vec2[num_non_zero] = k[0];  // add the column index
    _values[num_non_zero] = v;   // add the value
    // we just update the count of non-zeros at the curr. col-idx
    // note that the col-idx is automatically incremented
    _vec1[k[1] + 1] = ++num_non_zero;
  }

  // save memory and set flags
  _is_compressed = true;
  _entry_value_map.clear();
}

template <class T, StorageOrder Store>
void Matrix<T, Store>::_uncompress_col() {
#ifdef DEBUG
  std::cout << "Using COL-MAJOR uncompression.\n";
#endif
  // vec1 of length #cols + 1 -> col indices
  // vec2 of length #non-zero-elements -> row index
  // _values: length #non-zero-elements -> actual values
  std::size_t num_cols = _vec1.size() - 1;
  for (std::size_t col_idx = 0; col_idx < num_cols; ++col_idx) {
    for (std::size_t row_idx = _vec1[col_idx]; row_idx < _vec1[col_idx + 1];
         ++row_idx) {
      // we get the col number and the value accordingly
      _entry_value_map[std::array<std::size_t, 2>{_vec2[row_idx], col_idx}] =
          _values[row_idx];
    }
  }
  // save memory and set flags
  _is_compressed = false;
  _vec1.clear();
  _vec2.clear();
  _values.clear();
}

template <class T, StorageOrder Store>
const T& Matrix<T, Store>::_find_compressed_element_col(std::size_t row,
                                                        std::size_t col) const {
#ifdef DEBUG
  std::cout << "Using COL-MAJOR _find_compressed_element() const version.\n";
#endif
  for (std::size_t row_idx = _vec1[col]; row_idx < _vec1[col + 1]; ++row_idx) {
    if (_vec2[row_idx] == row) {
#ifdef DEBUG
      std::cout << "Found element.";
#endif
      return _values[row_idx];
    }
    return 0;
  }
}

template <class T, StorageOrder Store>
T& Matrix<T, Store>::_find_compressed_element_col(std::size_t row,
                                                  std::size_t col) {
#ifdef DEBUG
  std::cout
      << "Using COL-MAJOR _find_compressed_element() non-const version.\n";
#endif
  for (std::size_t row_idx = _vec1[col]; row_idx < _vec1[col + 1]; ++row_idx) {
    if (_vec2[row_idx] == row) {
#ifdef DEBUG
      std::cout << "Found element.";
#endif
      return _values[row_idx];
    }
    throw std::invalid_argument(
        "Trying to modify a zero-element in compressed format. Uncompress "
        "first");
  }
}
template <class T, StorageOrder Store>
std::vector<T> Matrix<T, Store>::_matrix_vector_col(std::vector<T> vec) const {
  for (const auto& i : _vec1) std::cout << i << " ";
  std::vector<T> res;
  // #rows = max value in the row-index vector
  std::size_t num_rows = *max_element(_vec2.begin(), _vec2.end());
  res.resize(num_rows + 1, 0);
  // iterate through the colums
  for (int col_idx = 0; col_idx < _vec1.size() - 1; ++col_idx) {
    for (std::size_t row_idx = _vec1[col_idx]; row_idx < _vec1[col_idx + 1];
         ++row_idx) {
      res[_vec2[row_idx]] += (vec[col_idx] * _values[row_idx]);
    }
  }
  return res;
}

}  // namespace algebra
#endif
