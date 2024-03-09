#ifndef STEP_SIZE_FACTORY_HPP
#define STEP_SIZE_FACTORY_HPP
#include "GetPot.hpp"
#include "StepSizeAbstract.hpp"
#include "StepSizeArmijo.hpp"
#include "StepSizeExponential.hpp"
#include "StepSizeInverseDecay.hpp"
#include "StepSizeParams.hpp"

enum class Methods { Armijo, Exponential, InverseDecay };
const std::unordered_map<std::string, Methods> methodMap = {
    {"Armijo", Methods::Armijo},
    {"Exponential", Methods::Exponential},
    {"InverseDecay", Methods::InverseDecay}};

// Function to convert string to enum class
Methods stringToMethod(const std::string& str) {
  auto it = methodMap.find(str);
  if (it != methodMap.end()) {
    return it->second;
  } else {
    throw std::invalid_argument("Invalid method string: " + str);
  }
}
template <int Size, typename Type>
class StepSizeFactory {
 public:
  // NOTE: need to use pointers to make polymorphism work...
  static StepSizeAbstract<Size, Type>* create_step_size(GetPot& gp) {
    Methods method = stringToMethod(gp("step_size/method", "Armijo"));
    switch (method) {
      case Methods::Armijo: {
        std::cout << "Using Armijo step size method." << std::endl;
        ArmijoParams params(gp);
        return new StepSizeArmijo<Size, Type>(params.alpha_0, params.sigma,
                                              params.verbose);
      }
      case Methods::Exponential: {
        std::cout << "Using Exponential step size method." << std::endl;
        ExpAndInverseDecayParams params_exp(gp);
        return new StepSizeExponential<Size, Type>(
            params_exp.alpha_0, params_exp.mu, params_exp.verbose);
      }
      case Methods::InverseDecay: {
        std::cout << "Using InverseDecay step size method." << std::endl;
        ExpAndInverseDecayParams params_inverse(gp);
        return new StepSizeInverseDecay<Size, Type>(
            params_inverse.alpha_0, params_inverse.mu, params_inverse.verbose);
      }
      default: {
        std::cout << "Choice for step size invalid, using Armijo by default."
                  << std::endl;
        ArmijoParams params_def(gp);
        return new StepSizeArmijo<Size, Type>(
            params_def.alpha_0, params_def.sigma, params_def.verbose);
      }
    }
  };
};

#endif  // STEP_SIZE_FACTORY_HPP