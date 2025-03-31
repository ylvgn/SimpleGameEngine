#pragma once

#include "Quat4.h"
#include "Mat4.h"

namespace sge { namespace Math {

#if 0
#pragma mark ========= Sphere3 ============
#endif
template<class T>
struct Sphere3 {
	using Vec3 = Vec3<T>;
public:
	Sphere3() = default;
	Sphere3(const Vec3& center_, T radius_)
		: center(center_)
		, radius(radius_)
	{}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Sphere3(c={}, r={})", center, radius);
	}

	Vec3	center;
	T		radius;
}; // Sphere3


#if 0
#pragma mark ========= Triangle3 ============
#endif
template<class T>
struct Triangle3 {
	using Vec3 = Vec3<T>;
public:
	Triangle3() = default;
	Triangle3(const Vec3& v0_, const Vec3& v1_, const Vec3& v2_)
		: v0(v0_)
		, v1(v1_)
		, v2(v2_)
	{}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Triangle3(v0={}, v1={}, v2={})", v0, v1, v2);
	}

	Vec3 v0, v1, v2;
}; // Triangle3


#if 0
#pragma mark ========= Plane3 ============
#endif
template<class T>
struct Plane3 {
	using Vec3 = Vec3<T>;
public:
	Plane3() = default;
	Plane3(const Vec3& normal_, T distance_)
		: normal(normal_)
		, distance(distance_) 
	{}

	Plane3(const Vec3& normal_, const Vec3& point_)
		: normal(normal_)
		, distance(normal_.dot(point_))
	{}

	Plane3(const Triangle3<T>& tri) { setByTriangle(tri); }

	void set(const Vec3& normal_, T distance_) {
		normal = normal_;
		distance = distance_;
	}

	void setByTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2) {
		normal = (v1 - v0).cross(v2 - v0).normal();
		distance = normal.dot(v0);
	}

	void setByTriangle(const Triangle3<T>& tri) {
		setByTriangle(tri.v0, tri.v1, tri.v2);
	}

	T dot(const Vec3& point) const { return normal.dot(point) - distance; }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Plane3(nl={}, dist={})", normal, distance);
	}

	Vec3	normal;
	T		distance; // distance from origin
}; // Plane3


#if 0
#pragma mark ========= Cylinder3 ============
#endif
template<class T>
struct Cylinder3 {
	using Vec3 = Vec3<T>;
public:
	Cylinder3() = default;
	Cylinder3(const Vec3& start_, const Vec3& end_, const T& radius_)
		: start(start_)
		, end(end_)
		, radius(radius_)
	{}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Cylinder3(st={}, ed={}, r={})", start, end, radius);
	}

	Vec3	start;
	Vec3	end;
	T		radius;
}; // Cylinder3


#if 0
#pragma mark ========= Capsule3 ============
#endif
template<class T>
struct Capsule3 {
	using Vec3 = Vec3<T>;
public:
	Capsule3() = default;
	Capsule3(const Vec3& start_, const Vec3& end_, const T& radius_)
		: start(start_)
		, end(end_)
		, radius(radius_)
	{}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Capsule3(st={}, ed={}, r={})", start, end, radius);
	}

	Vec3	start;
	Vec3	end;
	T		radius;
}; // Capsule3


#if 0
#pragma mark ========= Line2 ============
#endif
template<class T>
struct Line2 {
	using Vec2 = Vec2<T>;
	using Vec3 = Vec3<T>;
public:
	Line2() = default;
	Line2(const Vec2& start_, const Vec2& end_)
		: start(start_)
		, end(end_)
	{}

	bool getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const;
	T	 distanceToPoint(const Vec2& pt) const;

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Line2(st={}, ed={})", start, end);
	}

	Vec2	start;
	Vec2	end;
}; // Line2


#if 0
#pragma mark ========= Line3 ============
#endif
template<class T>
struct Line3 {
	using Vec3 = Vec3<T>;
public:
	Line3() = default;
	Line3(const Vec3& start_, const Vec3& end_)
		: start(start_)
		, end(end_)
	{}

	bool getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const;
	T	 distanceToPoint(const Vec3& pt) const;

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Line3(st={}, ed={})", start, end);
	}

	Vec3	start;
	Vec3	end;
}; // Line3


//------
using Sphere3f		= Sphere3<float>;
using Sphere3d		= Sphere3<double>;

using Plane3f		= Plane3<float>;
using Plane3d		= Plane3<double>;

using Triangle3f	= Triangle3<float>;
using Triangle3d	= Triangle3<double>;

using Capsule3f		= Capsule3<float>;
using Capsule3d		= Capsule3<double>;

using Line2f		= Line2<float>;
using Line2d		= Line2<double>;

using Line3f		= Line3<float>;
using Line3d		= Line3<double>;
}} // namespace sge/Math


namespace sge {
SGE_FORMATTER(Math::Sphere3f)
SGE_FORMATTER(Math::Sphere3d)

SGE_FORMATTER(Math::Plane3f)
SGE_FORMATTER(Math::Plane3d)

SGE_FORMATTER(Math::Triangle3f)
SGE_FORMATTER(Math::Triangle3d)

SGE_FORMATTER(Math::Capsule3f)
SGE_FORMATTER(Math::Capsule3d)

SGE_FORMATTER(Math::Line2f)
SGE_FORMATTER(Math::Line2d)

SGE_FORMATTER(Math::Line3f)
SGE_FORMATTER(Math::Line3d)
} // namespace sge