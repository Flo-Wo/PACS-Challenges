#include <iostream>
#include <string>

#include "Matrix.hpp"
#include "ReadMatrix.hpp"
#include "StorageOrder.hpp"
using namespace algebra;

void test_file_reader(const std::string& file_name) {
  try {
    // Example usage to read matrix in Matrix Market format
    auto rowMatrix = read_matrix<double, StorageOrder::row>(file_name);
    std::cout << "reading row worked\n";
    for (const auto& [k, v] : rowMatrix) {
      std::cout << "[" << k[0] << ", " << k[1] << "] = " << v << std::endl;
    }

    auto colMatrix = read_matrix<double, StorageOrder::col>(file_name);
    std::cout << "reading col worked\n";
    for (const auto& [k, v] : colMatrix) {
      std::cout << "[" << k[0] << ", " << k[1] << "] = " << v << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void test_matrix_row(const std::string& file_name) {
  // type aliasing
  // using order_type = StorageOrder::row;
  using num_type = double;

  auto rowMatrix = read_matrix<num_type, StorageOrder::row>(file_name);

#ifdef DEBUG
  std::cout << "num_elements in map: " << rowMatrix.size() << "\n";
  for (std::size_t row = 0; row < 10; ++row) {
    // iterate row-wise using the hint
    std::cout << "\nrow = " << row << ", row + 1 = " << row + 1 << "\n";
    for (auto it = rowMatrix.lower_bound({row, 0});
         it != rowMatrix.upper_bound({row + 1, 0}); ++it) {
      std::cout << "it->first[0] = " << it->first[0] << "\n";
      std::cout << "it->first[1] = " << it->first[1] << "\n";
      std::cout << "it->second = " << it->second << "\n";
    }
  }
  std::cout << "END OF MAIN\n\n";
#endif
  auto my_matrix = Matrix<num_type, StorageOrder::row>(rowMatrix);

  std::cout << "Printing the matrix\n";
  std::cout << my_matrix;
  my_matrix.compress();
  std::cout << my_matrix;
}
void test_matrix_col() {}

void benchmark_multiplication() {}

int main(int argc, char* argv[]) {
  std::string file_name = "./lnsp_131.mtx";

  // Check if an argument was provided
  if (argc > 1) {
    // Use the provided filename
    file_name = argv[1];
  }
  // test_file_reader(file_name);
  test_matrix_row(file_name);
  return 0;
}