#ifndef STEP_SIZE_EXP_HPP
#define STEP_SIZE_EXP_HPP

#include <concepts>
#include <functional>
#include <iostream>
#include <type_traits>

#include "Eigen/Dense"
#include "StepSizeAbstract.hpp"
template <int Size, typename Type>
class StepSizeExponential : public StepSizeAbstract<Size, Type> {
 public:
  StepSizeExponential(double alpha_0, double mu, bool verbose = false) {
    if (!(alpha_0 > 0)) {
      std::cout << "alpha_0 has to be larger than zero, currently " << alpha_0
                << std::endl;
      exit(0);
    }
    this->_alpha_0 = alpha_0;
    if (!(0 < mu)) {
      std::cout << "mu has to be > 0, currently " << mu << std::endl;
      exit(0);
    }
    this->_mu = mu;
    this->_verbose = verbose;
  };
  // no dynamic memery allocation
  ~StepSizeExponential() = default;

  double get_stepsize(
      Eigen::Matrix<Type, Size, 1>& x_curr, const unsigned int k_iter,
      std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
      std::function<
          Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
          grad_obj_func) const override {
    double step_size = this->_alpha_0 * exp((-1) * (this->_mu * k_iter));
    if (this->_verbose) {
      std::cout << "Linesearch ended, alpha = " << step_size << std::endl;
    }
    return step_size;
  };

 private:
  double _alpha_0;
  double _mu;
  bool _verbose;
};
#endif  // STEP_SIZE_EXP_HP