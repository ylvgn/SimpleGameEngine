#pragma once

#include "MyVec2_Basic.h"

namespace sge {

template<typename T> using MyVec2 = MyVec2_Basic<T>;

using MyVec2f = MyVec2<float>;
using MyVec2d = MyVec2<double>;

}