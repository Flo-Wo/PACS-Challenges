#include "StepSizeArmijo.hpp"

template <int Size, typename Type>
StepSizeArmijo<Size, Type>::StepSizeArmijo(double alpha_0, double sigma) {
  if (!(alpha_0 > 0)) {
    std::cout << "alpha_0 has to be larger than zero" << std::endl;
    exit(0);
  }
  this->_alpha_0 = alpha_0;
  if (0 < sigma and sigma < 0.5) {
    std::cout << "sigma has to be in (0, 0.5)" << std::endl;
  }
  this->_sigma = sigma;
}

template <int Size, typename Type>
StepSizeArmijo<Size, Type>::~StepSizeArmijo() {}

template <int Size, typename Type>
double StepSizeArmijo<Size, Type>::get_stepsize(
    Eigen::Matrix<Type, Size, 1>& x_curr,
    Eigen::Matrix<Type, Size, 1> (*obj_func)(Type),
    Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(
        Eigen::Matrix<Type, Size, 1>)) {
  double curr_alpha(this->_alpha_0);
  // lambda function to emulate the condition
  auto condition = [&](double alpha) {
    return (obj_func(x_curr) -
            obj_func(x_curr - alpha * grad_obj_func(x_curr))) >=
           this->_sigma * alpha * grad_obj_func(x_curr).norm();
  };
  while (!condition(curr_alpha)) {
    curr_alpha /= 2;
  }
  return curr_alpha;
}
