#include <iostream>
#include <string>

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

int main() {
  std::string file_name = "./lnsp_131.mtx";
  test_file_reader(file_name);
  return 0;
}