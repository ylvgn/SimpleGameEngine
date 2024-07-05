#pragma once

#include "MyVec4_Basic.h"

namespace sge {

template<typename T> using MyVec4 = MyVec4_Basic<T>;

using MyVec4f = MyVec4<float>;
using MyVec4d = MyVec4<double>;

}