#include "MathGeometry.h"

namespace sge { namespace Math {

template<class T>
bool Line2<T>::getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const {
	Line3<T> line3(Vec3::s_xy0(start), Vec3::s_xy0(end));
	Vec3 closestPt;
	if (!line3.getClosestPoint(closestPt, Vec3::s_xy0(inPoint)))
		return false;

	outPoint = closestPt.xy();
	return true;
}

template<class T>
T Line2<T>::distanceToPoint(const Vec2& pt) const {
	Vec2 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return Math::inf<T>();
}

template<class T>
bool Line3<T>::getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const {
	auto line = end - start;
	auto sqrLength = line.sqrLength();

	if (Math::equals0(sqrLength))
		return false;

	auto length = Math::sqrt(sqrLength);
	auto u = line / length; // normalized line
	auto v = inPoint - start;

	auto w = u.dot(v);
	w = Math::clamp(w, T(0), length);

	outPoint = start + u * w;
	return true;
}

template<class T>
T Line3<T>::distanceToPoint(const Vec3& pt) const {
	Vec3 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return Math::inf<T>();
}

#define	E(T) \
	template struct Line2<T>; \
	template struct Line3<T>; \
	template struct Plane3<T>; \
	template struct Sphere3<T>; \
	template struct Cylinder3<T>; \
	template struct Capsule3<T>; \
//----
	E(float)
	E(double)
#undef E

}} // namespace sge/Math
