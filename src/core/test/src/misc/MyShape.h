#pragma once
#include "MyReflection.h"

namespace sge {

class MyShape : public MyObject {
	MY_TYPEOF_TIBASEINIT_DEFINE(MyShape, MyObject)
public:
	f32 x;
	f32 y;
};

class MyCircle : public MyShape {
	MY_TYPEOF_TIBASEINIT_DEFINE(MyCircle, MyShape)
public:
	f64 r;
	Vector<int> testVectorInt;
	Vector<String> testVectorStr;
	Vector<Vector<int>> testVectorVectorInt;
};

class MyRect : public MyShape {
	MY_TYPEOF_TIBASEINIT_DEFINE(MyRect, MyShape)
public:
	u32 w;
	u32 h;
};

} // namespace