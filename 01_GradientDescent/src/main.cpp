#include <complex>
#include <fstream>
#include <iostream>
#include <string>

#include "DescentDirectionFactory.hpp"
#include "Eigen/Dense"
#include "GetPot.hpp"
#include "StepSizeAbstract.hpp"
#include "StepSizeFactory.hpp"
#include "StoppingConditionBase.hpp"
#include "gradient_descent.hpp"

void printHelp() {
  std::cout << "Use -p <filename.dat> to load user parameters" << std::endl;
}

template <int vec_size, typename vec_type>
vec_type obj_func(Eigen::Matrix<vec_type, vec_size, 1> const& x) {
  return x[0] * x[1] + 4 * pow(x[0], 4) + pow(x[1], 2) + 3 * x[0];
};

template <int vec_size, typename vec_type>
Eigen::Matrix<vec_type, vec_size, 1> grad_obj_func(
    Eigen::Matrix<vec_type, vec_size, 1> const& x) {
  Eigen::Matrix<vec_type, vec_size, 1> a;
  // NOTE: this is eigen 3.3.9
  a << x[1] + 16 * pow(x[0], 3) + 3, x[0] + 2 * x[1];
  // this is Eigen 3.4.0
  // {{x[1] + 16 * pow(x[0], 3) + 3, x[0] + 2 * x[1]}};
  return a;
};

// function returning a function which approximates the derivative using finite
// differences
template <int vec_size, typename vec_type>
std::function<const Eigen::Matrix<vec_type, vec_size, 1>(
    const Eigen::Matrix<vec_type, vec_size, 1>&)>
derivative_fd(
    const std::function<vec_type(Eigen::Matrix<vec_type, vec_size, 1> const&)>
        obj_func,
    double h) {
  // Compute diff via finite differences
  // note, we bind the function via reference and the h explicitly, otherwise
  // h=0
  auto finite_diff = [&, h](Eigen::Matrix<vec_type, vec_size, 1> const& x)
      -> Eigen::Matrix<vec_type, vec_size, 1> {
    Eigen::Matrix<vec_type, vec_size, 1> first_der;

    // pertubate each component individually and use central differences
    for (int i = 0; i < vec_size; ++i) {
      Eigen::Matrix<vec_type, vec_size, 1> pert =
          h * Eigen::Matrix<vec_type, vec_size, 1>::Unit(i);
#ifdef DEBUG
      std::cout << "i = " << i << std::endl;
      std::cout << "h = " << h << std::endl;
      std::cout << "test = " << Eigen::Matrix<vec_type, vec_size, 1>::Unit(i)
                << std::endl;
      std::cout << "h_vec = " << pert << std::endl;
      std::cout << "f(x+h) = " << obj_func(x + pert) << std::endl;
#endif
      first_der[i] = (obj_func(x + pert) - obj_func(x - pert)) / (2 * h);
    }
    return first_der;
  };
  // return the lambda function and type it correctly
  return std::function<Eigen::Matrix<vec_type, vec_size, 1>(
      Eigen::Matrix<vec_type, vec_size, 1> const&)>(finite_diff);
};

int main(int argc, char** argv) {
  GetPot cl(argc, argv);
  if (cl.search(2, "-h", " -help")) {
    printHelp();
    return 0;
  };
  std::string filename = cl.follow("opti_options.dat", "-p");
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Unable to open file " << filename << std::endl;
    return 1;
  }
  GetPot gp(filename);  // read the file in a getpot object

  double tol_res = gp("optimization/tol_res", 1.e-6);
  double tol_step_length = gp("optimization/tol_res", 1.e-6);
  bool verbose = gp("optimization/opti_verbose", true);
  unsigned int max_iter = gp("optimization/max_iter", 1000);

  // template parameters
  constexpr int vec_size{2};
  using vec_type = double;

  // get the abstract methods for step size and stopping criterion
  StepSizeAbstract<vec_size, vec_type>* step_size =
      StepSizeFactory<vec_size, vec_type>::create_step_size(gp);

  DescentDirectionAbstract<vec_size, vec_type>* descent =
      DescentDirectionFactory<vec_size, vec_type>::create_descent(gp);

  StoppingConditionBase<vec_size, vec_type> stop_cond{tol_res, tol_step_length,
                                                      max_iter};

  Eigen::Matrix<vec_type, vec_size, 1> x_start;  //{{0.0, 0.0}};
  x_start << 0.0, 0.0;

  // Create std::function objects from the function pointers
  std::function<vec_type(Eigen::Matrix<vec_type, vec_size, 1> const&)>
      obj_func_std = obj_func<vec_size, vec_type>;

  std::string grad_method = gp("gradient/computation/method", "analytical");
  std::function<Eigen::Matrix<vec_type, vec_size, 1>(
      Eigen::Matrix<vec_type, vec_size, 1> const&)>
      grad_obj_func_std;
  if (grad_method == "analytical") {
    std::cout << "Using Analytical derivative." << std::endl;
    // analytical derivative
    grad_obj_func_std = grad_obj_func<vec_size, vec_type>;
  } else {
    double h = gp("gradient/computation/h_finite_difference", 0.001);
    std::cout << "Using numerical derivative (FD) with h = " << h << std::endl;
    grad_obj_func_std = derivative_fd<vec_size, vec_type>(obj_func_std, h);
  }
  std::cout << "Starting the optimization" << std::endl;
  Eigen::Matrix<vec_type, vec_size, 1> x_sol =
      gradient_descent<vec_size, vec_type>(obj_func_std, grad_obj_func_std,
                                           x_start, stop_cond, step_size,
                                           descent, verbose);
  std::cout << "solution = \n" << x_sol << "\n" << std::endl;
  std::cout << "Finished the optimization" << std::endl;
  return 0;
}