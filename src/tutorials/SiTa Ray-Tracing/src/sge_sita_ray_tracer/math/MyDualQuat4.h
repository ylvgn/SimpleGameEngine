#pragma once

#include "MyDualQuat4_Basic.h"

namespace sge {

template<typename T> using MyDualQuat4 = MyDualQuat4_Basic<T>;

using MyDualQuat4f = MyDualQuat4<float>;
using MyDualQuat4d = MyDualQuat4<double>;

}