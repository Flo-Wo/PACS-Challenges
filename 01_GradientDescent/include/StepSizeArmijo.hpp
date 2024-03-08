#ifndef STEP_SIZE_ARMIJO_HPP
#define STEP_SIZE_ARMIJO_HPP

#include <iostream>

#include "Eigen/Dense"
#include "StepSizeAbstract.hpp"

template <int Size, typename Type>
class StepSizeArmijo : public StepSizeAbstract<Size, Type> {
 public:
  StepSizeArmijo(double alpha_0, double sigma);
  ~StepSizeArmijo();
  double get_stepsize(Eigen::Matrix<Type, Size, 1>& x_curr,
                      Eigen::Matrix<Type, Size, 1> (*obj_func)(Type),
                      Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(
                          Eigen::Matrix<Type, Size, 1>));

 private:
  double _alpha_0;
  double _sigma;
};
#endif  // STEP_SIZE_ARMIJO_HPP