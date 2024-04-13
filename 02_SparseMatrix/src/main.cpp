#include <iostream>
#include <string>

#include "Matrix.hpp"
#include "ReadMatrix.hpp"
#include "Utilities.hpp"
#include "chrono.hpp"

using namespace algebra;

void test_file_reader(const std::string& file_name) {
  try {
    // Example usage to read matrix in Matrix Market format
    auto matrix_mapping = read_matrix<double, StorageOrder::row>(file_name);
    std::cout << "reading row worked\n";
    for (const auto& [k, v] : matrix_mapping) {
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

template <class T, StorageOrder Order>
void test_matrix(const std::string& file_name) {
  std::cout << "===========================\n";
  std::cout << "\n\nTest case for " << Order << "\n";

  auto matrix_mapping = read_matrix<T, Order>(file_name);
  auto my_matrix = Matrix<T, Order>(matrix_mapping);

  std::cout << "Printing the matrix\n";
  std::cout << my_matrix;
  // should yield 0 and 4.0
  T val0 = my_matrix(0, 3);  // not present
  T val4 = my_matrix(3, 3);  // present
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

template <class T, StorageOrder Order>
void test_multiplication(const std::string& file_name) {
  auto matrix_mapping = read_matrix<T, Order>(file_name);
  auto my_matrix = Matrix<T, Order>(matrix_mapping);
  std::vector<T> to_multiply = {42, -17, 100, 0, 73, -5, 21, 8, -33, 55};

  auto res_uncomp = my_matrix * to_multiply;
  std::cout << "\nUncompressed Multiplication worked\n";
  for (auto i : res_uncomp) std::cout << i << ' ';
  my_matrix.compress();
  std::cout << my_matrix << "\n";
  auto res_comp = my_matrix * to_multiply;
  std::cout << "\nCompressed Multiplication worked\n";
  for (auto i : res_comp) std::cout << i << ' ';
}

template <class T, StorageOrder Order>
void benchmark_multiplication() {
  std::string file_name = "./lnsp_131.mtx";
  Timings::Chrono timer;
  // std::vector<T> to_multiply = _generate_random_vector<T>(131);

  std::cout << Order << "-MAJOR UNCOMPRESSED Multiplication took:\n";
  auto matrix_mapping = read_matrix<T, Order>(file_name);
  auto my_matrix = Matrix<T, Order>(matrix_mapping);

  std::vector<T> to_multiply = _generate_random_vector<T>(131);
  timer.start();
  auto res_raw = my_matrix * to_multiply;
  timer.stop();
  std::cout << timer;

  std::cout << Order << "-MAJOR COMPRESSED Compression took:\n";
  timer.start();
  my_matrix.compress();
  timer.stop();
  std::cout << timer;

  std::cout << Order << "-MAJOR COMPRESSED Multiplication took:\n";
  timer.start();
  auto res_compressed = my_matrix * to_multiply;
  timer.stop();
  std::cout << timer;
}

template <class T, StorageOrder Order>
void large_benchmark_multiplication(std::size_t num_runs) {
  std::string file_name = "./lnsp_131.mtx";
  Timings::Chrono timer;
  auto matrix_mapping = read_matrix<T, Order>(file_name);

  auto matrix_raw = Matrix<T, Order>(matrix_mapping);
  auto matrix_compressed = Matrix<T, Order>(matrix_mapping);
  matrix_compressed.compress();

  // save the times
  double total_time_raw = 0.0;
  double total_time_compressed = 0.0;

  for (std::size_t i = 0; i < num_runs; ++i) {
    std::vector<T> to_multiply = _generate_random_vector<T>(131);

    timer.start();
    auto res_raw = matrix_raw * to_multiply;
    timer.stop();
    total_time_raw += timer.wallTime();

    timer.start();
    auto res_compressed = matrix_compressed * to_multiply;
    timer.stop();
    total_time_compressed += timer.wallTime();
  }

  // Calculate average times
  double avg_time_raw = total_time_raw / num_runs;
  double avg_time_compressed = total_time_compressed / num_runs;

  // Report average times
  std::cout << "Large Benchmark Test for " << Order << "\n";
  std::cout << "Average time for UNCOMPRESSED Multiplication: " << avg_time_raw
            << " micro-seconds\n";
  std::cout << "Average time for COMPRESSED Multiplication: "
            << avg_time_compressed << " micro-seconds\n";
}

int main(int argc, char* argv[]) {
  using type_format = double;
  std::string file_name = "./lnsp_131.mtx";

  // Check if an argument was provided
  if (argc > 1) {
    // Use the provided filename
    file_name = argv[1];
  }
  // Test if the filereader works
  // test_file_reader(file_name);

  // Test if constructor, compression and call operator work
  // test_matrix<type_format, StorageOrder::row>(file_name);
  // test_matrix<type_format, StorageOrder::col>(file_name);

  // Test if multiplication works
  // test_multiplication<type_format, StorageOrder::row>(file_name);
  // test_multiplication<type_format, StorageOrder::col>(file_name);

  // Benchmark test for the matrix-vector multiplication
  benchmark_multiplication<type_format, StorageOrder::row>();
  benchmark_multiplication<type_format, StorageOrder::col>();

  // Large benchmark test with lots of runs
  // large_benchmark_multiplication<type_format, StorageOrder::row>(1);
  // large_benchmark_multiplication<type_format, StorageOrder::col>(1);
  return 0;
}