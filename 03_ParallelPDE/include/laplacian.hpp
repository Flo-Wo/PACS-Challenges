#include <functional>
#include <iostream>

#include "Eigen/Dense"

template <typename Type>
void jacobi_update(
    const Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>& U_curr,
    Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>& U_next,
    Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>& f_matrix, int nx,
    int ny, double h) {
  // TODO: omp pragma
  // #pragma omp parallel for collapse(2)
  for (int i = 1; i < nx - 1; ++i) {
    for (int j = 1; j < ny - 1; ++j) {
      // jacobi rule with a forcing term
      U_next(i, j) =
          0.25 * (U_curr(i - 1, j) + U_curr(i + 1, j) + U_curr(i, j - 1) +
                  U_curr(i, j + 1) + h * h * f_matrix(i, j));
    }
  }
  // TODO compute the local error and report it
  // return err;
}

// use to create local versions of the grid
template <typename Type>
Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> init_grid(int n_grid_x,
                                                              int n_grid_y) {
  return Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>::Zero(n_grid_x,
                                                                   n_grid_y);
}

// use to create local versions of the grid
template <typename Type>
Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> init_f_matrix(
    int n_grid_x, int n_grid_y, double h,
    std::function<double(double, double)> f) {
  Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> f_matrix(n_grid_x,
                                                               n_grid_y);

  std::cout << "eval f";
  for (int i = 0; i < n_grid_x; ++i) {
    for (int j = 0; j < n_grid_y; ++j) {
      f_matrix(i, j) = f(i * h, j * h);
      std::cout << f_matrix(i, j) << " ";
    }
  }
  std::cout << "\n";

  return f_matrix;
}
