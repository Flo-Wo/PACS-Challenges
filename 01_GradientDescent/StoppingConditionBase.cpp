#include "StoppingConditionBase.hpp"

template <int Size, typename Type>
StoppingConditionBase<Size, Type>::StoppingConditionBase(double tol_res,
                                                         double tol_step_length,
                                                         int max_iter) {
  this->_tol_res = tol_res;
  this->_tol_step_length = tol_step_length;
  this->_max_iter = max_iter;
}
// no dynamic memory allocation
template <int Size, typename Type>
StoppingConditionBase<Size, Type>::~StoppingConditionBase() {}

template <int Size, typename Type>
bool StoppingConditionBase<Size, Type>::stop(
    Eigen::Matrix<Type, Size, 1> const& x_curr,
    Eigen::Matrix<Type, Size, 1> const& x_prev,
    Eigen::Matrix<Type, Size, 1> (*obj_func)(Type),
    Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(Eigen::Matrix<Type, Size, 1>),
    int const k) {
  if (((x_curr - x_prev).norm() < this->_tol_step_length) or
      (grad_obj_func(x_curr).norm() < this->_tol_res) or
      (k > this->_max_iter)) {
    return true;
  }
  return false;
}

template <int Size, typename Type>
double StoppingConditionBase<Size, Type>::get_tol_res() {
  return this->_tol_res;
}
template <int Size, typename Type>
double StoppingConditionBase<Size, Type>::get_tol_step_length() {
  return this->_tol_step_length;
}
template <int Size, typename Type>
int StoppingConditionBase<Size, Type>::get_max_iter() {
  return this->_max_iter;
}
