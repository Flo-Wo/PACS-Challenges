#include <iostream>

#include "Eigen/Dense"
#include "StepSizeAbstract.hpp"
#include "StoppingConditionBase.hpp"
#include "gradient_descent.hpp"
// later --> factory
#include "StepSizeArmijo.hpp"

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

int main() {
  double tol_res = 1.e-16;
  double tol_step_length = 1.e-12;
  int max_iter = 1000;

  // template parameters
  constexpr int vec_size{2};
  using vector_type = double;

  // get the abstract methods for step size and stopping criterion
  StepSizeArmijo<vec_size, vector_type> step_size{0.45, 0.1};
  StoppingConditionBase<vec_size, vector_type> stop_cond{
      tol_res, tol_step_length, max_iter};

  // TODO: check for init --> correct?
  Eigen::Matrix<vector_type, vec_size, 1> x_start{{0.0, 0.0}};

  // Create std::function objects from the function pointers
  std::function<vector_type(Eigen::Matrix<vector_type, vec_size, 1> const&)>
      obj_func_std = obj_func<vec_size, vector_type>;

  std::function<Eigen::Matrix<vector_type, vec_size, 1>(
      Eigen::Matrix<vector_type, vec_size, 1> const&)>
      grad_obj_func_std = grad_obj_func<vec_size, vector_type>;

  std::cout << "Starting the optimization" << std::endl;
  Eigen::Matrix<vector_type, vec_size, 1> x_sol =
      gradient_descent<vec_size, vector_type>(
          obj_func_std, grad_obj_func_std, x_start, stop_cond, step_size, true);

  std::cout << "solution = " << x_sol << std::endl;
  std::cout << "Finished the optimization" << std::endl;
  return 0;
}

/*
TODO:
- check by reference, check const types
- step size factory
- diff step size functions
- read options at run time --> call the factory

- generalize the descent method
*/