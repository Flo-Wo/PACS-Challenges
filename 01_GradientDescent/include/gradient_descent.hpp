#ifndef GRADIENT_DESCENT_HPP
#define GRADIENT_DESCENT_HPP

#include <functional>
#include <iostream>

#include "Eigen/Dense"
#include "StepSizeAbstract.hpp"
#include "StoppingConditionBase.hpp"

template <int Size, typename Type>
Eigen::Matrix<Type, Size, 1>& gradient_descent(
    std::function<Type(const Eigen::Matrix<Type, Size, 1>&)> obj_func,
    std::function<
        Eigen::Matrix<Type, Size, 1>(const Eigen::Matrix<Type, Size, 1>&)>
        grad_obj_func,
    // Eigen::Matrix<Type, Size, 1> (*obj_func)(Type),
    // Eigen::Matrix<Type, Size, 1> (*grad_obj_func)(Eigen::Matrix<Type, Size,
    // 1>),
    Eigen::Matrix<Type, Size, 1> const& x_start,
    StoppingConditionBase<Size, Type> const& stop_cond,
    StepSizeAbstract<Size, Type> const& step_size, bool const verbose = false);
#endif  // GRADIENT_DESCENT_HPP