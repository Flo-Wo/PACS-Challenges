#ifndef DESCENT_DIR_FACTORY_HPP
#define DESCENT_DIR_FACTORY_HPP
#include <algorithm>
#include <functional>
#include <memory>

#include "Eigen/Dense"
#include "GetPot.hpp"


//@note I sugget making type aliases to make the code more readeable and flexible. For instance:
// using Vector=Eigen::Matrix<Type, Size, 1>; 
// So that you can use Vector instead of Eigen::Matrix<Type, Size, 1> in the class definition

template <int Size, typename Type>
class DescentDirectionAbstract {
 public:
  // const version for the steepest descent
  virtual Eigen::Matrix<Type, Size, 1> get_dir(
      const Eigen::Matrix<Type, Size, 1>& grad_x_curr, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev) const {
    // NOTE: this is implement to avoid a warning, but could be dangerous since
    // it is hard to catch
    //@note if you implement a method that returns a value, even if it is meant to be a dummy method,
    // you should return a value, otherwise you will get a warning. Alternative: make the method pure virtual, After all
    // the  DescentDirectionAbstract class is Abstract and this method is meant to be overridden by the inheriting classes
    //virtual Eigen::Matrix<Type, Size, 1> get_dir(...) const = 0;
    //
  
    return Eigen::Matrix<Type, Size, 1>{};
  };
//@note I suppose you did not make the momentum const since it need to modify a state of the momentum class
// Personally I think that this in not a good design, since the get_dir method should be const, but the momentum should be modified
// by making mutable the variable members used internally by the get dir method in the momentum class.
  // non-const version for the momentum
  virtual Eigen::Matrix<Type, Size, 1> get_dir(
      const Eigen::Matrix<Type, Size, 1>& grad_x_curr, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev) {
    // NOTE: this is implement to avoid a warning, but could be dangerous since
    // it is hard to catch
    return Eigen::Matrix<Type, Size, 1>{};
  };

  // enables us to call the setter within the descent method, so we don't need
  // to pass the data loader into the method, only if we need to store sth.
  // NOTE: provide const and non-const for both of the inheriting classes
  virtual void set_grad_x0(Eigen::Matrix<Type, Size, 1> grad_x0) const {};
  virtual void set_grad_x0(Eigen::Matrix<Type, Size, 1> grad_x0){};

//@note I stongly suggest making the destructor virtual, since you have virtual methods in the class
//  virtual ~DescentDirectionAbstract()=default;

};

template <int Size, typename Type>
class SteepestDescent : public DescentDirectionAbstract<Size, Type> {
 public:
  SteepestDescent(){};
  Eigen::Matrix<Type, Size, 1> get_dir(
      const Eigen::Matrix<Type, Size, 1>& grad_x_curr, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev) const override {
    return (-1) * step_size * grad_x_curr;
    //@note Very convoluted way of writing return -step_size * grad_x_curr;
  };
};

template <int Size, typename Type>
class Momentum : public DescentDirectionAbstract<Size, Type> {
 public:
 //@note You like onvoluted coding. _momentum is a member of type Eigen::Matrix<Type, Size, 1>,
 // so, by defualt, it will be initilised with the defualt constructor of the Eigen::Matrix class
 // you do no need to initialize it in the constructor explicitely, since it is what the compiler would normally do anyway. 
 // you can simplify the constructor to:
// Momentum(double memory, bool fixed_memory = true) : _memory(memory) {... all the rest}
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
      const Eigen::Matrix<Type, Size, 1>& grad_x_curr, double step_size,
      const Eigen::Matrix<Type, Size, 1>& x_curr,
      const Eigen::Matrix<Type, Size, 1>& x_prev) override {
    // get the previous direction
    Eigen::Matrix<Type, Size, 1> dir = this->_momentum;
    // update the internal momentum with the current gradient
    this->_momentum = this->_get_memory(step_size) * this->_momentum -
                      step_size * grad_x_curr;
    return dir;
  };

 private:
  std::function<double(double)> _get_memory;
  double _memory;
  //@note make the momentum mutable to be able to modify it in the get_dir method
  // and make that method const
  // mutable Eigen::Matrix<Type, Size, 1> _momentum;
  Eigen::Matrix<Type, Size, 1> _momentum;
};

template <int Size, typename Type>
class DescentDirectionFactory {
 public:
  static std::unique_ptr<DescentDirectionAbstract<Size, Type>> create_descent(
      GetPot& gp) {
    std::string grad_dir =
        gp("gradient/computation/direction/method", "steepest");
    if (grad_dir == "steepest") {
      std::cout << "Using Steepest Descent direction." << std::endl;
      return std::make_unique<SteepestDescent<Size, Type>>();
    } else {
      std::cout << "Using Momentum Descent direction." << std::endl;
      return std::make_unique<Momentum<Size, Type>>(
          gp("gradient/direction/memory", 0.9),
          gp("gradient/direction/fixed_memory", true));
    }
  }
};
#endif  // DESCENT_DIR_FACTORY_HPP