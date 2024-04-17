#ifndef MATRIX_ROW_IMPLEMENTATION_HPP
#define MATRIX_ROW_IMPLEMENTATION_HPP

#include "Matrix.hpp"

template <class T, StorageOrder Store>
void Matrix<T, Store>::_compress_row() {
#ifdef DEBUG
  std::cout << "Using ROW-MAJOR compression to CSR.\n";
#endif
  // vec1 of length #rows + 1 -> row indices
  // vec2 of length #non-zero-elements -> column index
  // _values: length #non-zero-elements -> actual values

  // #rows + 1 = highest row-number + 2
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
const T Matrix<T, Store>::_find_compressed_element_row(std::size_t row,
                                                       std::size_t col) const {
#ifdef DEBUG
  std::cout << "Using ROW-MAJOR _find_compressed_element() const version.\n ";
#endif
  for (std::size_t col_idx = _vec1[row]; col_idx < _vec1[row + 1]; ++col_idx) {
    if (_vec2[col_idx] == col) {
#ifdef DEBUG
      std::cout << "Found element: " << row << ", " << col << ".\n";
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
      std::cout << "Found element: " << row << ", " << col << ".\n";
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
#endif