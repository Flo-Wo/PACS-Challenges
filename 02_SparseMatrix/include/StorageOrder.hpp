#ifndef STORAGE_ORDER_HPP
#define STORAGE_ORDER_HPP
#include <iostream>
namespace algebra {
enum StorageOrder { row, col };

template <typename T>
struct ColOrderComparator {
  bool operator()(const std::array<std::size_t, 2>& a,
                  const std::array<std::size_t, 2>& b) const {
    return (a[1] < b[1]) || ((a[1] == b[1]) && (a[0] < b[0]));
  }
};

template <typename T>
struct RowOrderComparator {
  bool operator()(const std::array<std::size_t, 2>& a,
                  const std::array<std::size_t, 2>& b) const {
    // #ifdef DEBUG
    //     std::cout << "Row compator is used\n";
    //     std::cout << "a[0] " << a[0] << ", b[0] " << b[0] << "\n";
    //     std::cout << "a[1] " << a[1] << ", b[1] " << b[1] << "\n";
    //     std::cout << "smaller: " << (a[0] < b[0]) << "\n";
    //     std::cout << "smaller 2: "
    //               << ((a[0] < b[0]) || ((a[0] == b[0]) && (a[1] < b[1]))) <<
    //               "\n";
    // #endif
    return (a[0] < b[0]) || ((a[0] == b[0]) && (a[1] < b[1]));
  }
};

}  // namespace algebra
#endif