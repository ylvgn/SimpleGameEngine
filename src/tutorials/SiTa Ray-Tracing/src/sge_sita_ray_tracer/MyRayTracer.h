#pragma once

#include <sge_sita_ray_tracer/math/MyMat4.h>
#include <sge_sita_ray_tracer/math/MyRay3.h>

namespace sge {

//template<typename T>
class MyRayTracer {
public:
	using T = float;
	using ElementType = T;
	using MyMat4 = MyMat4<T>;
	using MyRay3 = MyRay3<T>;

	void init(const MyVec2f& screenSize, const MyMat4& projMatrix, const MyMat4& modelview);

	MyRay3 getRay(T x, T y);

	MyMat4 _projMatrix;
	MyMat4 _modelview;

	MyMat4 _invProjMatrix;
	MyMat4 _invModelview;

	MyVec2f	_screenSize;
};

}