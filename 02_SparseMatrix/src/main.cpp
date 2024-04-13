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

template <StorageOrder Order>
void test_matrix(const std::string& file_name) {
  std::cout << "===========================\n";
  std::cout << "\n\nTest case for " << Order << "\n";
  using num_type = double;

  auto rowMatrix = read_matrix<num_type, Order>(file_name);
  auto my_matrix = Matrix<num_type, Order>(rowMatrix);

  std::cout << "Printing the matrix\n";
  std::cout << my_matrix;
  // should yield 0 and 4.0
  num_type val0 = my_matrix(0, 3);  // not present
  num_type val4 = my_matrix(3, 3);  // present
  std::cout << "val0 = " << val0 << ", val4 = " << val4 << "\n";

  // Test compression + call operator
  my_matrix.compress();
  std::cout << "COMPRESSION WORKED\n";
  std::cout << my_matrix;

  val0 = my_matrix(0, 3);  // not present
  val4 = my_matrix(3, 3);  // present
  std::cout << "val0 = " << val0 << ", val4 = " << val4 << "\n";

  // Test uncompress + call operator
  my_matrix.uncompress();
  std::cout << "UNCOMPRESSION WORKED\n";
  std::cout << my_matrix;
}

template <StorageOrder Order>
void test_multiplication(const std::string& file_name) {
  using num_type = double;

  auto rowMatrix = read_matrix<num_type, Order>(file_name);
  auto my_matrix = Matrix<num_type, Order>(rowMatrix);
  std::vector<num_type> to_multiply = {42, -17, 100, 0, 73, -5, 21, 8, -33, 55};

  auto res_uncomp = my_matrix * to_multiply;
  std::cout << "\nUncompressed Multiplication worked\n";
  for (auto i : res_uncomp) std::cout << i << ' ';
  my_matrix.compress();
  std::cout << my_matrix << "\n";
  auto res_comp = my_matrix * to_multiply;
  std::cout << "\nCompressed Multiplication worked\n";
  for (auto i : res_comp) std::cout << i << ' ';
}

void benchmark_multiplication() {}

int main(int argc, char* argv[]) {
  std::string file_name = "./lnsp_131.mtx";

  // Check if an argument was provided
  if (argc > 1) {
    // Use the provided filename
    file_name = argv[1];
  }
  // test_file_reader(file_name);
  // test_matrix<StorageOrder::row>(file_name);
  // test_matrix<StorageOrder::col>(file_name);
  test_multiplication<StorageOrder::row>(file_name);
  test_multiplication<StorageOrder::col>(file_name);
  return 0;
}