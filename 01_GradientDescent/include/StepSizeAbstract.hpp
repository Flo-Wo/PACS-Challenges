#ifndef STEP_SIZE_ABSTRACT_HPP
#define STEP_SIZE_ABSTRACT_HPP
#include "Eigen/Dense"

template <int Size, typename Type>
class StepSizeAbstract {
 public:
  // purely virtual function, to serve as an interface/abstract class
  virtual double get_stepsize(Eigen::Matrix<Type, Size, 1>& x_curr,
                              Eigen::Matrix<Type, Size, 1> (*obj_func)(Type),
                              Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(
                                  Eigen::Matrix<Type, Size, 1>)) = 0;
};
#endif  // STEP_SIZE_ABSTRACT_HPP