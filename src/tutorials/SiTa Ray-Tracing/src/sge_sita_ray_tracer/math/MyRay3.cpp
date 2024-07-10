#include "MyRay3.h"

namespace sge {

template<typename T>
MyRay3<T> MyRay3<T>::unprojectFromInverseMatrix (const MyMat4& invProj,
												 const MyMat4& invModelview,
												 const MyVec2& pointOnScreen,
												 const MyVec2& screenSize)
{
	auto v2HCS = pointOnScreen / screenSize * T(2) - T(1);
	v2HCS.y = -v2HCS.y;

	auto v4PCSSt = invProj.mulPoint(MyVec4(v2HCS.x, v2HCS.y, 0, 1));
	auto v4PCSEd = invProj.mulPoint(MyVec4(v2HCS.x, v2HCS.y, 1, 1));

	auto v4LCSSt = invModelview.mulPoint(v4PCSSt);
	auto v4LCSEd = invModelview.mulPoint(v4PCSEd);

	MyRay3 o;
	o.origin = v4LCSSt.toVec3();
	o.dir = (v4LCSEd.toVec3() - o.origin).normalize();
	return o;
}

template<typename T>
bool MyRay3<T>::raycast(HitResult& outResult, Plane3 plane, T maxDistance) {
// https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
	outResult.reset();

	T s = dir.dot(plane.normal);

	if (Math::equals0(s)) {
		return false; // parallel to the plane
	}

	if (s > 0) {
		return false; // back face
	}

	auto distance = -(origin.dot(plane.normal) - plane.distance) / s;
	if (distance < 0 || distance > maxDistance) {
		return false;
	}

	outResult.distance	= distance;
	outResult.point		= origin + dir * distance;
	outResult.normal	= plane.normal;
	outResult.hasResult = true;
	return true;
}

template<typename T>
bool MyRay3<T>::raycast(HitResult& outResult, Sphere3 sphere, T maxDistance) {
	outResult.reset();

	const auto& C = sphere.center;
	const auto& r = sphere.radius;

#if false // no need check
	if (Math::equals0(r) || r < 0)
		return false;
#endif

	auto OC = C - origin;
	T s = OC.dot(dir);

	if (s < 0)
		return false;

	auto M = origin + dir * s;
	auto CM = M - C;
	auto d2 = CM.sqrMagnitude();
	auto r2 = r * r;

	if (r2 < d2)
		return false;

	if (Math::equals0(d2)) {
		T distance	= OC.magnitude() - r;
		if (distance < 0 || distance > maxDistance)
			return false;

		outResult.distance  = distance;
		outResult.normal	= -OC.normalize();
		outResult.point		= origin + dir * outResult.distance;
		outResult.hasResult = true;
		return true;
	}

	auto d = Math::sqrt(d2);
	if (Math::equals(d, r)) {
		T distance = d;
		if (distance < 0 || distance > maxDistance)
			return false;

		outResult.distance	= distance;
		outResult.normal	= CM.normalize();
		outResult.point		= M;
		outResult.hasResult = true;
		return true;
	}

	{
		auto offset = Math::sqrt(r2 - d2);
		T distance = (M - origin).magnitude() - offset;
		if (distance < 0 || distance > maxDistance)
			return false;

		outResult.distance	= distance;
		outResult.point		= origin + dir * outResult.distance;
		outResult.normal	= (outResult.point - C).normalize();
		outResult.hasResult = true;
	}
	return true;
}

// explicit specialization, and it will help that no need impl template member function in .h file
template struct MyRay3<float>;
template struct MyRay3<double>;

}