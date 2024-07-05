#pragma once

#include "MyLinearTransform_Basic.h"

namespace sge {

template<typename T> using MyLinearTransform = MyLinearTransform_Basic<T>;

using MyLinearTransformf = MyLinearTransform<float>;
using MyLinearTransformd = MyLinearTransform<double>;

}