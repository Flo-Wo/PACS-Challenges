#ifndef STEP_SIZE_ARMIJO_HPP
#define STEP_SIZE_ARMIJO_HPP

#include <concepts>
#include <functional>
#include <iostream>
#include <type_traits>

#include "Eigen/Dense"
#include "StepSizeAbstract.hpp"
template <typename ConvertToDouble>
concept Type = std::convertible_to<ConvertToDouble, double>;

template <int Size, typename Type>
class StepSizeArmijo : public StepSizeAbstract<Size, Type> {
 public:
  StepSizeArmijo(double alpha_0, double sigma) {
    if (!(alpha_0 > 0)) {
      std::cout << "alpha_0 has to be larger than zero" << std::endl;
      exit(0);
    }
    this->_alpha_0 = alpha_0;
    if (!(0 < sigma and sigma < 0.5)) {
      std::cout << "sigma has to be in (0, 0.5)" << std::endl;
      exit(0);
    }
    this->_sigma = sigma;
  };
  // no dynamic memery allocation
  ~StepSizeArmijo(){};

  double get_stepsize(
      Eigen::Matrix<Type, Size, 1>& x_curr,
      std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
      std::function<
          Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
          grad_obj_func) const {
    double curr_alpha(this->_alpha_0);
    auto condition = [&](Type alpha) {
      auto x_curr_eval = obj_func(x_curr);
      auto x_next_eval = obj_func(x_curr - alpha * grad_obj_func(x_curr));
      return (x_curr_eval - x_next_eval) >=
             this->_sigma * alpha * grad_obj_func(x_curr).norm();
    };
    while (!condition(curr_alpha)) {
      curr_alpha /= 2;
    }
    return curr_alpha;
  };

 private:
  double _alpha_0;
  double _sigma;
};
#endif  // STEP_SIZE_ARMIJO_HPP