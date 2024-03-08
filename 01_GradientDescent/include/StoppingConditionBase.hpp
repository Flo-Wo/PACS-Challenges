#ifndef STOPPING_CONDITION_BASE_HPP
#define STOPPING_CONDITION_BASE_HPP
#include "Eigen/Dense"

template <int Size, typename Type>
class StoppingConditionBase {
 public:
  StoppingConditionBase(double tol_res, double tol_step_length, int max_iter);
  ~StoppingConditionBase();
  virtual bool stop(Eigen::Matrix<Type, Size, 1> const& x_curr,
                    Eigen::Matrix<Type, Size, 1> const& x_prev,
                    // obj_fun not needed, but potentially for overriding
                    Eigen::Matrix<Type, Size, 1> (*obj_func)(Type),
                    Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(
                        Eigen::Matrix<Type, Size, 1>),
                    int const k);
  double get_tol_res();
  double get_tol_step_length();
  int get_max_iter();

 private:
  double _tol_step_length;
  double _tol_res;
  int _max_iter;
};

#endif  // STOPPING_CONDITION_BASE_HPP