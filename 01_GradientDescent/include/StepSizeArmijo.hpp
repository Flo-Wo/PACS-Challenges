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
  StepSizeArmijo(double alpha_0, double sigma, bool verbose = false) {
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
    this->_verbose = verbose;
  };
  // no dynamic memery allocation
  ~StepSizeArmijo(){};

  double get_stepsize(
      Eigen::Matrix<Type, Size, 1>& x_curr, const int k_iter,
      std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
      std::function<
          Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
          grad_obj_func) const {
    double curr_alpha(this->_alpha_0);

    // we precompute all alpha-independent terms
    Type x_curr_eval = obj_func(x_curr);
    Eigen::Matrix<Type, Size, 1> curr_dir = grad_obj_func(x_curr);
    Type rhs_without_alpha = this->_sigma * curr_dir.norm();

    auto condition = [&](Type alpha) {
      Type x_next_eval = obj_func(x_curr - alpha * curr_dir);
#ifdef DEBUG
      std::cout << "alpha = " << alpha << std::endl;
      std::cout << "left-hand side: " << x_curr_eval - x_next_eval << std::endl;
      std::cout << "right-hand side: " << alpha * rhs_without_alpha
                << std::endl;
#endif
      return (x_curr_eval - x_next_eval) >= alpha * rhs_without_alpha;
    };

    // actual loop to check for the satisfied condition, count iterations
    int it = 0;
    while (!condition(curr_alpha)) {
      curr_alpha /= 2;
      ++it;
    }
    if (this->_verbose) {
      std::cout << "Linesearch ended, alpha = " << curr_alpha
                << " #step = " << it << std::endl;
    }

    return curr_alpha;
  };

 private:
  double _alpha_0;
  double _sigma;
  bool _verbose;
};
#endif  // STEP_SIZE_ARMIJO_HPP