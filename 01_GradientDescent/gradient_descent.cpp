#include "gradient_descent.hpp"

template <int Size, typename Type>
Eigen::Matrix<Type, Size, 1>& gradient_descent(
    std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
    std::function<
        Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
        grad_obj_func,
    // Eigen::Matrix<Type, Size, 1>(*obj_func)(Type),
    // Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(Eigen::Matrix<Type, Size,
    // 1>),
    Eigen::Matrix<Type, Size, 1> const& x_start,
    StoppingConditionBase<Size, Type> const& stop_cond,
    StepSizeAbstract<Size, Type> const& step_size, bool const verbose) {
  int k_iter = 0;
  Eigen::Matrix<Type, Size, 1> x_prev{x_start};
  Eigen::Matrix<Type, Size, 1> x_curr{x_start};

  // catch trivial case
  if (grad_obj_func(x_start).norm() < stop_cond.get_tol_res()) {
    return x_curr;
  }

  // make single step to simplify the stopping condition, i.e. change x_k
  x_curr = x_prev - step_size.get_stepsize(x_prev, obj_func, grad_obj_func) *
                        obj_func_grad(x_prev);
  k_iter++;

  while (!stop_cond.stop(x_curr, x_prev, obj_func, grad_obj_func, k_iter)) {
    if (verbose) {
      std::cout << "Curr it: " << k_iter
                << "Grad(f)(x_k) = " << grad_obj_func(x_curr) << std::endl;
    }
    x_prev = x_curr;  // copy?
    x_curr = x_prev - step_size.get_stepsize(x_prev, obj_func, grad_obj_func) *
                          obj_func_grad(x_prev);
    k_iter++;
  }

  // if opti ended to due max iter --> should give a warning
  if (k_iter == stop_cond.get_max_iter()) {
    std::cout << "Maximum number of iterations reached, algo probably did not "
                 "converge"
              << std::endl;
  }

  return x_curr;
}