#ifndef GRADIENT_DESCENT_HPP
#define GRADIENT_DESCENT_HPP

#include <functional>
#include <iostream>

#include "Eigen/Dense"
#include "StepSizeAbstract.hpp"
#include "StoppingConditionBase.hpp"

template <int Size, typename Type>
Eigen::Matrix<Type, Size, 1>& gradient_descent(
    std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
    std::function<
        Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
        grad_obj_func,
    // Eigen::Matrix<Type, Size, 1> (*obj_func)(Type),
    // Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(Eigen::Matrix<Type, Size,
    // 1>),
    Eigen::Matrix<Type, Size, 1> const& x_start,
    StoppingConditionBase<Size, Type> const& stop_cond,
    StepSizeAbstract<Size, Type> const& step_size, bool const verbose = false) {
  int k_iter = 0;
  Eigen::Matrix<Type, Size, 1> x_prev{x_start};
  Eigen::Matrix<Type, Size, 1> x_curr{x_start};

  // catch trivial case
  if (grad_obj_func(x_start).norm() < stop_cond.get_tol_res()) {
    return x_curr;
  }
  std::cout << "Reached non-trivial case" << std::endl;

  // make single step to simplify the stopping condition, i.e. change x_k
  double step_size_test =
      step_size.get_stepsize(x_prev, obj_func, grad_obj_func);
  x_curr = x_prev - step_size.get_stepsize(x_prev, obj_func, grad_obj_func) *
                        grad_obj_func(x_prev);

  std::cout << "step_size = " << step_size_test << std::endl;
  std::cout << "x_curr = " << x_curr << std::endl;
  std::cout << "x_prev = " << x_prev << std::endl;
  k_iter++;

  std::cout << "starting the loop" << std::endl;
  while (!stop_cond.stop(x_curr, x_prev, obj_func, grad_obj_func, k_iter)) {
    std::cout << "\n\nstep_size = " << step_size_test << std::endl;
    std::cout << "x_curr = " << x_curr << std::endl;
    std::cout << "x_prev = " << x_prev << std::endl;
    if (verbose) {
      std::cout << "Curr it: " << k_iter << "\n"
                << "Grad(f)(x_k) = " << grad_obj_func(x_curr) << std::endl;
    }
    x_prev = x_curr;  // copy?
    x_curr = x_prev - step_size.get_stepsize(x_prev, obj_func, grad_obj_func) *
                          grad_obj_func(x_prev);
    k_iter++;
  }

  std::cout << "#Iterations = " << k_iter << std::endl;
  // if opti ended to due max iter --> should give a warning
  if (k_iter == stop_cond.get_max_iter()) {
    std::cout << "Maximum number of iterations reached, algo probably did not "
                 "converge"
              << std::endl;
  }

  return x_curr;
};
#endif  // GRADIENT_DESCENT_HPP