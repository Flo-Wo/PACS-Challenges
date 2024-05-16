#include <math.h>
#include <mpi.h>
#include <omp.h>

#include <cmath>
#include <iostream>

#include "laplacian.hpp"

int main(int argc, char* argv[]) {
  // numerical attributes + forcing function
  int max_iter = 1000;
  auto forcing_func = [](double x, double y) {
    std::cout << "x = " << x << ", y = " << y << "\n";
    return 8 * M_PI * M_PI * std::sin(M_PI * x) * std::cos(M_PI * y);
  };

  // discretization
  int k = 2;
  int n_grid_points = std::pow(2, k);
  double h = 1.0 / (n_grid_points - 1);
  std::cout << "h = " << h << ", n_grid_points = " << n_grid_points << "\n";
  using matrix_type = double;

  // MPI values
  int rank, size;

  // Init MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // compute local sizes and matrices
  // TODO: check for non perfect cases -> needs to be adjusted
  // TODO: account for the offset of the rank
  std::cout << "size = " << size << ", rank = " << rank << "\n";
  int local_n_grid_x = n_grid_points / size;
  std::cout << "local_n_grid_x: " << local_n_grid_x << "\n";
  int local_n_grid_y = n_grid_points;
  std::cout << "local_n_grid_y: " << local_n_grid_y << "\n";

  auto grid_curr = init_grid<matrix_type>(local_n_grid_x, local_n_grid_y);
  auto grid_next = init_grid<matrix_type>(local_n_grid_x, local_n_grid_y);

  // check if this init is correct -> correctly cut
  auto f_matrix = init_f_matrix<matrix_type>(local_n_grid_x, local_n_grid_y, h,
                                             forcing_func);

  std::cout << "f_matrix: \n";
  std::cout << f_matrix << "\n";
  // main loop
  for (int iter = 0; iter < max_iter; ++iter) {
    // Exchange boundary data between neighboring processes

    // send top boundary condition to process above
    if (rank > 0) {
      MPI_Sendrecv(&grid_curr(0, 0), local_n_grid_y, MPI_DOUBLE, rank - 1, 0,
                   &grid_curr(0, local_n_grid_y), local_n_grid_y, MPI_DOUBLE,
                   rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    // send bottom boundary condition to process below
    if (rank < size - 1) {
      MPI_Sendrecv(&grid_curr(local_n_grid_x - 1, 0), local_n_grid_y,
                   MPI_DOUBLE, rank + 1, 0,
                   &grid_curr(local_n_grid_x - 1, local_n_grid_y),
                   local_n_grid_y, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD,
                   MPI_STATUS_IGNORE);
    }

    // perform the actual update with OpenMP in parallel
    jacobi_update<matrix_type>(grid_curr, grid_next, f_matrix, local_n_grid_x,
                               local_n_grid_y, h);

    // perform the actual update -> one could do this in a smarter way with
    // two flip flop pointer between grid_curr and grid_next
    grid_curr.block(1, 1, local_n_grid_x - 2, local_n_grid_y - 2) =
        grid_next.block(1, 1, local_n_grid_x - 2, local_n_grid_y - 2);

    // Synchronize to ensure all processes have completed communication
    MPI_Barrier(MPI_COMM_WORLD);
    // std::cout << "iter = " << iter << "\n";
  }

  // cleanup
  MPI_Finalize();

  // Print final result
  std::cout << "Final Result:" << std::endl;
  std::cout << grid_next << std::endl;
  return 0;
}