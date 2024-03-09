#ifndef STEP_SIZE_FACTORY_HPP
#define STEP_SIZE_FACTORY_HPP
#include "StepSizeAbstract.hpp"
#include "StepSizeArmijo.hpp"
#include "StepSizeExponential.hpp"
#include "StepSizeInverseDecay.hpp"

enum Options = {Armijo, Exponential, InverseDecay};
template <int Size, typename Type>
class StepSizeFactory {
 public:
  static StepSizeAbstract<Size, Type> create_step_size(Options choice,
                                                       StepSizeParams params) {
    switch (choice) {
      case Armijo:
        // load options
        // return Armijo
        break;

      case Exponential:
        break;

      case InverseDecay:
        break;

      default:
        std::cout << "Choice for step size invalid, using Armijo by default."
                  << std::endl;
        // TODO
    }
  };
};

#endif  // STEP_SIZE_FACTORY_HPP