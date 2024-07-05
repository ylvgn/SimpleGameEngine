#pragma once

#include "MyVec3_Basic.h"

namespace sge {

template<typename T> using MyVec3 = MyVec3_Basic<T>;

using MyVec3f = MyVec3<float>;
using MyVec3d = MyVec3<double>;

}