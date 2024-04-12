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

void test_matrix_constructor(const std::string& file_name) {
  // type aliasing
  using order_type = StorageOrder::row;
  using num_type = double;

  auto rowMatrix = read_matrix<num_type, order_type>(file_name);
  auto my_matrix = Matrix<num_type, order_type>(rowMatrix);
}

int main() {
  std::string file_name = "./lnsp_131.mtx";
  // test_file_reader(file_name);
  test_matrix_constructor(file_name);
  return 0;
}