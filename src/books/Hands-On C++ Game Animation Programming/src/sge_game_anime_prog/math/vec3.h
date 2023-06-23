#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/log/Log.h>
#include <sge_core/math/Math.h>
#include "vec2.h"

namespace sge {

//FYI: https://gabormakesgames.com/vectors.html

template<typename T>
struct TVec3 {

	union {
		struct { T x, y, z; };
		T v[3];
	};

	inline TVec3() = default;
	inline TVec3(const T& x_, const T& y_, const T& z_) : x(x_), y(y_), z(z_) {}

	inline static TVec3 s_zero()	{ return TVec3(0,0,0); }
	inline static TVec3 s_one()		{ return TVec3(1,1,1); }

	inline static TVec3 s_forward()	{ return TVec3( 0, 0, 1); }
	inline static TVec3 s_back()	{ return TVec3( 0, 0,-1); }
	inline static TVec3 s_up()		{ return TVec3( 0, 1, 0); }
	inline static TVec3 s_down()	{ return TVec3( 0,-1, 0); }
	inline static TVec3 s_right()	{ return TVec3( 1, 0, 0); }
	inline static TVec3 s_left()	{ return TVec3(-1, 0, 0); }

	inline bool equals(const TVec3& r, const T& epsilon = Math::epsilon<T>()) const;
	inline bool equals0(               const T& epsilon = Math::epsilon<T>()) const;

	inline TVec3 operator+ (const TVec3& r) const { return TVec3(x+r.x, y+r.y, z+r.z); }
	inline TVec3 operator- (const TVec3& r) const { return TVec3(x-r.x, y-r.y, z-r.z); }
	inline TVec3 operator* (const TVec3& r) const { return TVec3(x*r.x, y*r.y, z*r.z); } // non-uniform scale operator
	inline TVec3 operator/ (const TVec3& r) const { return TVec3(x/r.x, y/r.y, z/r.z); }

	inline TVec3 operator+ (const T& s)		const { return TVec3(x+s, y+s, z+s); }
	inline TVec3 operator- (const T& s)		const { return TVec3(x-s, y-s, z-s); }
	inline TVec3 operator* (const T& s)		const { return TVec3(x*s, y*s, z*s); }
	inline TVec3 operator/ (const T& s)		const { return TVec3(x/s, y/s, z/s); }

	inline void operator+= (const TVec3& r) { x+=r.x; y+=r.y; z+=r.z; }
	inline void operator-= (const TVec3& r) { x-=r.x; y-=r.y; z-=r.z; }
	inline void operator*= (const TVec3& r) { x*=r.x; y*=r.y; z*=r.z; }
	inline void operator/= (const TVec3& r) { x/=r.x; y/=r.y; z/=r.z; }

	inline void operator+= (const T& s)		{ x+=s; y+=s; z+=s; }
	inline void operator-= (const T& s)		{ x-=s; y-=s; z-=s; }
	inline void operator*= (const T& s)		{ x*=s; y*=s; z*=s; }
	inline void operator/= (const T& s)		{ x/=s; y/=s; z/=s; }

	inline TVec3 operator-() const { return TVec3(-x,-y,-z); }

	inline			T& operator[](int i)		{ return v[i]; }
	inline const	T& operator[](int i) const	{ return v[i]; }

	inline	TVec2<T> xy() const { return TVec2<T>(x,y); }
	inline	TVec2<T> xz() const { return TVec2<T>(x,z); }
	inline	TVec2<T> yz() const { return TVec2<T>(y,z); }

	inline T		dot(const TVec3& r)		const { return (x*r.x) + (y*r.y) + (z*r.z); }
	inline TVec3	cross(const TVec3& r)	const { return TVec3(y*r.z - z*r.y, z*r.x - x*r.z, x*r.y - y*r.x); }

	inline TVec3	normalize() const { T m = magnitude(); return Math::equals0(m) ? s_zero() : (*this / m); }

	inline T		magnitude()				const { return Math::sqrt(sqrMagnitude()); }
	inline T		sqrMagnitude()			const { return dot(*this); }
	inline T		lenSq()					const { return sqrMagnitude(); }
	inline T		len()					const { return magnitude(); }

	// angle means radians, not degrees
	inline T angle(const TVec3& r) const {
		// Finding the angle between the two vectors would be expensive
		T m1 = magnitude();
		T m2 = r.magnitude();

		if (Math::equals0(m1) || Math::equals0(m2))
			return 0;

		T cosTheta = dot(r) / (m1 * m2);
		return Math::acos(cosTheta);
	}

	// project *this onto r
	inline TVec3 project(const TVec3& r) const {
		T m = r.magnitude();
		if (Math::equals0(m))
			return s_zero();

		// *this is considered as hypotenuse
		// so, s = ||hypotenuse|| * cos(theta) = dot(r) / ||r||

		T s = dot(r) / m;
		return r * s;
	}

	// rejection is perpendicular to projection
	inline TVec3 reject(const TVec3& r) const {
		TVec3 proj = project(r);
		return *this - proj;
	}

	// bounce-like reflect
	inline TVec3 reflect(const TVec3& r) const {
		TVec3 proj = project(r);
		if (proj.equals0())
			return s_zero();

		TVec3 projx2 = proj+proj;
		return *this - projx2;
	}

	// linear interpolatation
	inline TVec3 lerp(const TVec3& to, const T& t) const {
//		return *this*(1.0f-t) + to*t;
//		return *this + (to - *this)*t;
//		return TVec3(x + ( (to.x-x)*t ), y + ( (to.y-y)*t ), z + ( (to.z-z)*t ));

		return TVec3(Math::lerp(x, to.x, t),
					 Math::lerp(y, to.y, t),
					 Math::lerp(z, to.z, t));
	}

	// spherical linear interpolatation: constant in velocity
	inline TVec3 slerp(const TVec3& to_, const T& t) const {

		// when t is close to 0, slerp will yield unexpected results
		if (Math::equals0(Math::abs(t - 0.01))) {
			return lerp(to_, t);
		}

		TVec3 from  = this->normalize();
		TVec3 to    = to_.normalize();

		T theta		= from.angle(to);
		T sinTheta	= Math::sin(theta);

		T a = Math::sin( (1.0f-t)*theta ) / sinTheta;
		T b = Math::sin(t*theta) / sinTheta;
		return (from*a) + (to*b);
	}

	// normalize lerp: non-constant in velocity
	inline TVec3 nlerp(const TVec3& to, const T& t) const { return lerp(to, t).normalize(); }

	inline bool operator== (const TVec3& r) const { return x == r.x && y == r.y && z == r.z; }
	inline bool operator!= (const TVec3& r) const { return !(this->operator==(r)); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "[{}, {}, {}]", x, y, z);
	}
};

template<class T> inline
bool TVec3<T>::equals(const TVec3& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon);
}

template<class T> inline
bool TVec3<T>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon);
}

template<>
struct TVec3<int> {
	using T = int;
	using This = TVec3<T>;
	union {
		struct { T x, y, z; };
		T v[3];
	};

	inline TVec3(const T& x_, const T& y_, const T& z_) : x(x_), y(y_), z(z_) {}
};

template<>
struct TVec3<unsigned int> {
	using T = unsigned int;
	using This = TVec3<T>;
	union {
		struct { T x, y, z; };
		T v[3];
	};

	inline TVec3(const T& x_, const T& y_, const T& z_) : x(x_), y(y_), z(z_) {}
};

using vec3f		= TVec3<float>;
using vec3d		= TVec3<double>;
using vec3i		= TVec3<int>;
using vec3ui	= TVec3<unsigned int>;

using vec3		= vec3f;

SGE_FORMATTER_T(class T, TVec3<T>)

}
