#ifndef STOPPING_CONDITION_BASE_HPP
#define STOPPING_CONDITION_BASE_HPP
#include <functional>

#include "Eigen/Dense"

template <int Size, typename Type>
class StoppingConditionBase {
 public:
  StoppingConditionBase(double tol_res, double tol_step_length,
                        unsigned int max_iter) {
    this->_tol_res = tol_res;
    this->_tol_step_length = tol_step_length;
    this->_max_iter = max_iter;
  };
  ~StoppingConditionBase(){};
  virtual bool stop(
      Eigen::Matrix<Type, Size, 1> const& x_curr,
      Eigen::Matrix<Type, Size, 1> const& x_prev,
      // obj_fun not needed, but potentially for overriding methods
      std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
      std::function<
          Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
          grad_obj_func,
      int const k) const {
    // condition given in the challenge
    if (((x_curr - x_prev).norm() < this->_tol_step_length) or
        (grad_obj_func(x_curr).norm() < this->_tol_res) or
        (k > this->_max_iter)) {
      return true;
    }
    return false;
  };
  double get_tol_res() const { return this->_tol_res; };
  double get_tol_step_length() const { return this->_tol_step_length; };
  int get_max_iter() const { return this->_max_iter; };

 private:
  double _tol_step_length;
  double _tol_res;
  unsigned int _max_iter;
};

#endif  // STOPPING_CONDITION_BASE_HPP