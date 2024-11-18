#include "MathGeometry.h"

namespace sge { namespace Math {

template<class T>
void Sphere3<T>::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Sphere3(c={}, r={})", center, radius);
}

template<class T>
void Triangle3<T>::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Triangle3(v0={}, v1={}, v2={})", v0, v1, v2);
}

template<class T>
void Plane3<T>::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Plane3(nl={}, dist={})", normal, distance);
}

template<class T>
void Cylinder3<T>::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Cylinder3(st={}, ed={}, r={})", start, end, radius);
}

template<class T>
void Capsule3<T>::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Capsule3(st={}, ed={}, r={})", start, end, radius);
}

template<class T>
void Line2<T>::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Line2(st={}, ed={})", start, end);
}

template<class T>
void Line3<T>::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Line3(st={}, ed={})", start, end);
}

#define	E(T) \
	template struct Line2<T>; \
	template struct Line3<T>; \
	template struct Plane3<T>; \
	template struct Sphere3<T>; \
	template struct Cylinder3<T>; \
	template struct Capsule3<T>; \
//-------------
	E(float)
	E(double)
#undef E

}} // namespace sge/Math
