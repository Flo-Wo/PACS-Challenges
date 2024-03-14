#ifndef GRADIENT_DESCENT_HPP
#define GRADIENT_DESCENT_HPP

#include <functional>
#include <iostream>

#include "DescentDirectionFactory.hpp"
#include "Eigen/Dense"
#include "StepSizeAbstract.hpp"
#include "StoppingConditionBase.hpp"

template <int Size, typename Type>
Eigen::Matrix<Type, Size, 1> gradient_descent(
    std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
    std::function<
        Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
        grad_obj_func,
    Eigen::Matrix<Type, Size, 1> const& x_start,
    StoppingConditionBase<Size, Type> const& stop_cond,
    StepSizeAbstract<Size, Type> const* step_size,
    DescentDirectionAbstract<Size, Type> const* descent,
    bool const verbose = false) {
  unsigned int k_iter = 0;
  Eigen::Matrix<Type, Size, 1> x_prev{x_start};
  Eigen::Matrix<Type, Size, 1> x_curr{x_start};

  Eigen::Matrix<Type, Size, 1> grad_xk{grad_obj_func(x_start)};
  // catch trivial case
  if (grad_xk.norm() < stop_cond.get_tol_res()) {
    if (verbose) {
      std::cout << "Starting point is the solution" << std::endl;
    }
    return x_curr;
  }

  // non-trivial case:
  // --> init the step size and start caching the descent directions
  double step_size_val =
      step_size->get_stepsize(x_prev, 0, obj_func, grad_obj_func);
  descent->set_grad_x0(grad_xk);
  // make single step to simplify the stopping condition, i.e. change x_k
  x_curr -= step_size_val * grad_xk;
  ++k_iter;

#ifdef DEBUG
  std::cout << "\n\nFirst iteration before the loop: " << std::endl;
  std::cout << "step size: " << step_size_val << std::endl;
  std::cout << "dir =  "
            << descent->get_dir(grad_xk, step_size_val, x_curr, x_prev)
            << std::endl;
  std::cout << "x_prev = " << x_prev << std::endl;
  std::cout << "x_curr = " << x_curr << std::endl;
#endif
  // actual descent algorithm
  while (!stop_cond.stop(x_curr, x_prev, obj_func, grad_obj_func, k_iter)) {
#ifdef DEBUG
    std::cout << "\n\nstep_size = " << step_size_val << std::endl;
    std::cout << "grad = " << grad_xk << std::endl;
    std::cout << "dir = "
              << descent->get_dir(grad_xk, step_size_val, x_curr, x_prev)
              << std::endl;
    std::cout << "x_prev = " << x_prev << "\n\n" << std::endl;
    std::cout << "x_curr = " << x_curr << std::endl;
#endif
    if (verbose and k_iter % 10 == 0) {
      std::cout << "Curr it: " << k_iter
                << ", ||∇(f)(x_k)|| = " << grad_xk.norm() << std::endl;
    }
    x_prev = x_curr;  // copy the Eigen matrix

    // compute the descent direction and the step size
    grad_xk = grad_obj_func(x_curr);
    step_size_val = step_size->get_stepsize(x_prev, 0, obj_func, grad_obj_func);
    x_curr -= step_size_val * grad_xk;
    ++k_iter;
  }

  if (verbose) {
    std::cout << "FINISHED: \n\t#Iterations = " << k_iter << std::endl;
    std::cout << "\tf(x) = " << obj_func(x_curr) << std::endl;
    std::cout << "\t||∇f(x)|| = " << grad_xk.norm() << std::endl;
  }

  // if opti ended to due max iter --> should give a warning
  if (k_iter == stop_cond.get_max_iter()) {
    std::cout << "Maximum number of iterations reached, algo probably did not "
                 "converge"
              << std::endl;
  }

  return x_curr;
};
#endif  // GRADIENT_DESCENT_HPP