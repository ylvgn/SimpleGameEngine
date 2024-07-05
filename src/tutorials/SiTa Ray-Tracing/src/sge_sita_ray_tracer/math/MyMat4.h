#pragma once

#include "MyMat4_Basic.h"

namespace sge {

template<typename T> using MyMat4 = MyMat4_Basic<T>;

using MyMat4f = MyMat4<float>;
using MyMat4d = MyMat4<double>;

}