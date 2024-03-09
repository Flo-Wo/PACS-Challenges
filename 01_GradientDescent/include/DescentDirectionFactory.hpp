#ifndef DESCENT_DIR_FACTORY_HPP
#define DESCENT_DIR_FACTORY_HPP
#include <algorithm>
#include <functional>

#include "Eigen/Dense"
#include "GetPot.hpp"

template <int Size, typename Type>
class DescentDirectionAbstract {
 public:
  // const version for the steepest descent
  virtual Eigen::Matrix<Type, Size, 1> get_dir(
      const Eigen::Matrix<Type, Size, 1>& grad_x_k, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev) const {};

  // non-const version for the momentum
  virtual Eigen::Matrix<Type, Size, 1> get_dir(
      const Eigen::Matrix<Type, Size, 1>& grad_x_k, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev){};

  // enables us to call the setter within the descent method, so we don't need
  // to pass the data loader into the method, only if we need to store sth.
  // NOTE: provide const and non-const for both of the inheriting classes
  virtual void set_grad_x0(Eigen::Matrix<Type, Size, 1> grad_x0) const {};
  virtual void set_grad_x0(Eigen::Matrix<Type, Size, 1> grad_x0){};
};

template <int Size, typename Type>
class SteepestDescent : public DescentDirectionAbstract<Size, Type> {
 public:
  SteepestDescent(){};
  Eigen::Matrix<Type, Size, 1> get_dir(
      const Eigen::Matrix<Type, Size, 1>& grad_x_k, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev) const override {
    return (-1) * step_size * grad_x_k;
  };
};

template <int Size, typename Type>
class Momentum : public DescentDirectionAbstract<Size, Type> {
 public:
  Momentum(double memory, bool fixed_memory = true)
      : _memory(memory), _momentum(Eigen::Matrix<Type, Size, 1>{}) {
    if (fixed_memory) {
      _get_memory = [this](double step_size) -> double {
        return this->_memory;
      };
    } else {
      _get_memory = [](double step_size) -> double {
        // cap the step size, to ensure a fast computation
        return (1 - std::min(std::max(step_size, 0.999), 0.0));
      };
    }
  };
  void set_grad_x0(Eigen::Matrix<Type, Size, 1> grad_x0) override {
    this->_momentum = grad_x0;
  }
  Eigen::Matrix<Type, Size, 1> get_dir(
      const Eigen::Matrix<Type, Size, 1>& grad_x_k, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev) override {
    // get the previous direction
    Eigen::Matrix<Type, Size, 1> dir = this->_momentum;
    // update the internal momentum with the current gradient
    this->_momentum =
        this->_get_memory(step_size) * this->_momentum - step_size * grad_x_k;
    return dir;
  };

 private:
  std::function<double(double)> _get_memory;
  double _memory;
  Eigen::Matrix<Type, Size, 1> _momentum;
};

template <int Size, typename Type>
class DescentDirectionFactory {
 public:
  static DescentDirectionAbstract<Size, Type>* create_descent(GetPot& gp) {
    std::string grad_dir = gp("gradient/direction/method", "steepest");
    if (grad_dir == "steepest") {
      std::cout << "Using Steepest Descent direction." << std::endl;
      return new SteepestDescent<Size, Type>();
    } else {
      std::cout << "Using Momentum Descent direction." << std::endl;
      return new Momentum<Size, Type>(
          gp("gradient/direction/memory", 0.9),
          gp("gradient/direction/fixed_memory", true));
    }
  }
};
#endif  // DESCENT_DIR_FACTORY_HPP