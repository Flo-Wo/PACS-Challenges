#include <iostream>

#include "GetPot.hpp"

struct ArmijoParams {
  ArmijoParams(double alpha_0, double sigma, bool verbose)
      : alpha_0(alpha_0), sigma(sigma), verbose(verbose) {}
  ArmijoParams(GetPot& gp)
      : alpha_0(gp("step_size/alpha_0", 0.9)),
        sigma(gp("step_size/armijo/sigma", 0.45)),
        verbose(gp("step_size/step_size_verbose", false)) {}

  double alpha_0;
  double sigma;
  bool verbose;
};

struct ExpAndInverseDecayParams {
  ExpAndInverseDecayParams(double alpha_0, double mu, bool verbose)
      : alpha_0(alpha_0), mu(mu), verbose(verbose) {}
  ExpAndInverseDecayParams(GetPot& gp)
      : alpha_0(gp("step_size/alpha_0", 1.0)),
        mu(gp("step_size/exponential_and_inverseDecay/mu", 0.2)),
        verbose(gp("step_size/step_size_verbose", false)) {}
  double alpha_0;
  double mu;
  bool verbose;
};