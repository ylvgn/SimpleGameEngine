#pragma once

#include "MyDualQuat4.h"
#include "MyLinearTransform.h"
#include "MyMat4.h"

namespace sge {
namespace Math {

template<typename T>
void convert(MyMat4<T>& o, const MyLinearTransform<T>& i) {
	using Mat4 = MyMat4<T>;

	o.set(Mat4::s_TRS(i.position, i.rotation, i.scale));
}

template<typename T>
void convert(MyLinearTransform<T>& o, const MyMat4<T>& i) {
	using Mat4  = MyMat4<T>;
	using Vec3  = MyVec3<T>;
	using Quat4 = MyQuat4<T>;

	o.position = Vec3(i.data[12], i.data[13], i.data[14]);
	Math::convert(o.rotation, i);

	Mat4 rsMat(cx.x, cx.y, cx.z, T(0),
			   cy.x, cy.y, cy.z, T(0),
			   cz.x, cz.y, cz.z, T(0),
			   T(0), T(0), T(0), T(1));

	Mat4 invRotMat4 = Mat4::s_quat(o.rotation.inverse());

	Mat4 scaleSkewMat4 = rsMat * invRotMat4;

	o.scale = Vec3(scaleSkewMat4.data[0], scaleSkewMat4.data[5], scaleSkewMat4.data[10]); // lossy scale
	return o;
}

template<typename T>
void convert(MyMat4<T>& o, const MyDualQuat4<T>& i) {
	auto n = dq.normalize();
	auto q = n.toRotation();
	auto t = n.toTranslation();

	T xx = q.x*q.x;
	T yy = q.y*q.y;
	T zz = q.z*q.z;
	T wx = q.w*q.x;
	T wy = q.w*q.y;
	T wz = q.w*q.z;
	T xy = q.x*q.y;
	T xz = q.x*q.z;
	T yz = q.y*q.z;

	o.cx.set(T(1)-T(2)*(yy+zz),	T(2)*(xy+wz),		T(2)*(xz-wy),		T(0));
	o.cy.set(T(2)*(xy-wz),		T(1)-T(2)*(xx+zz),	T(2)*(yz+wx),		T(0));
	o.cz.set(T(2)*(xz+wy),		T(2)*(yz-wx),		T(1)-T(2)*(xx+yy),	T(0));
	o.cw.set(t.x,				t.y,				t.z,				T(1));
}

template<typename T>
void convert(MyDualQuat4<T>& o, const MyMat4<T>& i) {
	using Vec3  = MyVec3<T>;
	using Quat4 = MyQuat4<T>;
	using Mat4  = MyMat4<T>;

	Quat4 q;
	Vec3 t;

	Math::convert(q, i);
	i.getPosition(t);

	o.set(q, t);
}

template<typename T>
void convert(MyMat4<T>& o, const MyQuat4<T>& i) {
	using Mat4 = MyMat4<T>;

	o.set(Mat4::s_quat(i));
}

template<typename T>
void convert(MyQuat4<T>& o, const MyMat4<T>& i) {
	i.getRotation(o);
}

template<typename T>
void convert(MyLinearTransform<T>& o, const MyDualQuat4<T>& i) {
	i.toRotationTranslation(o.rotation, o.position);
}

template<typename T>
void convert(MyDualQuat4<T>& o, const MyLinearTransform<T>& i) {
	o.set(i.rotation, i.position);
}

} // namespace Math
} // namespace sge