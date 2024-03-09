#include <fstream>
#include <iostream>
#include <string>

#include "Eigen/Dense"
#include "GetPot.hpp"
#include "StepSizeAbstract.hpp"
#include "StepSizeFactory.hpp"
#include "StoppingConditionBase.hpp"
#include "gradient_descent.hpp"

void printHelp() {
  std::cout << "Use -p <filename.dat> to load user parameters" << std::endl;
}

template <int size, typename vector_type>
vector_type obj_func(Eigen::Matrix<vector_type, size, 1> const& x) {
  return x[0] * x[1] + 4 * pow(x[0], 4) + pow(x[1], 2) + 3 * x[0];
};

template <int size, typename vector_type>
Eigen::Matrix<vector_type, size, 1> grad_obj_func(
    Eigen::Matrix<vector_type, size, 1> const& x) {
  Eigen::Matrix<vector_type, size, 1> a{
      {x[1] + 16 * pow(x[0], 3) + 3, x[0] + 2 * x[1]}};
  return a;
};

int main(int argc, char** argv) {
  GetPot cl(argc, argv);
  if (cl.search(2, "-h", " -help")) {
    printHelp();
    return 0;
  };
  // Search if we are giving —v version
  std::string filename = cl.follow("opti_options.dat", "-p");
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Unable to open file " << filename << std::endl;
    return 1;
  }
  GetPot gp(filename);  // read the file in a getpot object

  double tol_res = gp("optimization/tol_res", 1.e-6);
  double tol_step_length = gp("optimization/tol_res", 1.e-6);
  bool verbose = gp("optimization/opti_verbose", true);
  int max_iter = gp("optimization/max_iter", 1000);
  std::cout << gp("step_size/alpha_0", 0.9) << std::endl;

  // template parameters
  constexpr int vec_size{2};
  using vector_type = double;

  // get the abstract methods for step size and stopping criterion
  StepSizeAbstract<vec_size, vector_type>* step_size =
      StepSizeFactory<vec_size, vector_type>::create_step_size(gp);

  StoppingConditionBase<vec_size, vector_type> stop_cond{
      tol_res, tol_step_length, max_iter};

  Eigen::Matrix<vector_type, vec_size, 1> x_start{{0.0, 0.0}};

  // Create std::function objects from the function pointers
  std::function<vector_type(Eigen::Matrix<vector_type, vec_size, 1> const&)>
      obj_func_std = obj_func<vec_size, vector_type>;

  std::function<Eigen::Matrix<vector_type, vec_size, 1>(
      Eigen::Matrix<vector_type, vec_size, 1> const&)>
      grad_obj_func_std = grad_obj_func<vec_size, vector_type>;

  std::cout << "Starting the optimization" << std::endl;
  Eigen::Matrix<vector_type, vec_size, 1> x_sol =
      gradient_descent<vec_size, vector_type>(obj_func_std, grad_obj_func_std,
                                              x_start, stop_cond, step_size,
                                              verbose);

  std::cout << "solution = \n" << x_sol << "\n" << std::endl;
  std::cout << "Finished the optimization" << std::endl;
  return 0;
}

/*
TODO:
- check by reference, check const types
- generalize the descent method
*/