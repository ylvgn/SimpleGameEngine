#pragma once

#include "MyQuat4_Basic.h"

namespace sge {

template<typename T> using MyQuat4 = MyQuat4_Basic<T>;

using MyQuat4f = MyQuat4<float>;
using MyQuat4d = MyQuat4<double>;

}