
#ifndef MATRIX_COL_IMPLEMENTATION_HPP
#define MATRIX_COL_IMPLEMENTATION_HPP
#include "Matrix.hpp"

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
const T Matrix<T, Store>::_find_compressed_element_col(std::size_t row,
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
  }
  return 0;
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
  }
  throw std::invalid_argument(
      "Trying to modify a zero-element in compressed format. Uncompress "
      "first");
}
template <class T, StorageOrder Store>
std::vector<T> Matrix<T, Store>::_matrix_vector_col(std::vector<T> vec) const {
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
#endif