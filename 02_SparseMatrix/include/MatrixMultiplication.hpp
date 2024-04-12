#include <vector>

#include "Matrix.h"

namespace algebra {
template <class T, StorageOrder Store>
// TODO: decide at compile time which operation we use
// we need two version for row and col major
void multiplication(Matrix<T, Store> matrix, std::vector<T> vec){};
}  // namespace algebra