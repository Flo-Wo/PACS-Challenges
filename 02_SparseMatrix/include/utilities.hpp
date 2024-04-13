#ifndef UTILITIES_RANDOM_VEC_HPP
#define UTILITIES_RANDOM_VEC_HPP

#include <random>
#include <vector>

namespace algebra {

template <class T>
std::vector<T> _generate_random_vector(std::size_t size, int seed,
                                       double lower_bound = -10,
                                       double upper_bound = 10) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<T> dis(lower_bound, upper_bound);

  std::vector<T> random_vector(size);
  for (int i = 0; i < size; ++i) {
    random_vector[i] = dis(gen);
  }
  return random_vector;
}
}  // namespace algebra
#endif