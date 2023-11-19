#pragma once

#include "vec2.h"

namespace sge {
/*
	FYI: https://gabormakesgames.com/vectors.html
*/

template<typename T>
struct TVec3 {
	
	static const size_t kElementCount = 3;

	union {
		struct { T x, y, z; };
		T data[kElementCount];
	};

	SGE_INLINE TVec3() = default;
	SGE_INLINE TVec3(const T& x_, const T& y_, const T& z_) : x(x_), y(y_), z(z_) {}

	SGE_INLINE void set(const T& x_, const T& y_, const T& z_) { x = x_; y = y_; z = z_; }

	SGE_INLINE static TVec3 s_zero()	{ return TVec3(0,  0,  0); }
	SGE_INLINE static TVec3 s_one()		{ return TVec3(1,  1,  1); }
	SGE_INLINE static TVec3 s_forward() { return TVec3(0,  0,  1); }
	SGE_INLINE static TVec3 s_back()	{ return TVec3(0,  0, -1); }
	SGE_INLINE static TVec3 s_up()		{ return TVec3(0,  1,  0); }
	SGE_INLINE static TVec3 s_down()	{ return TVec3(0, -1,  0); }
	SGE_INLINE static TVec3 s_right()	{ return TVec3(1,  0,  0); }
	SGE_INLINE static TVec3 s_left()	{ return TVec3(-1, 0,  0); }

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const	{ SGE_ASSERT(i < kElementCount); return data[i]; }

	SGE_INLINE bool equals(const TVec3& r, const T& epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(const T& epsilon = Math::epsilon<T>()) const;

	SGE_INLINE TVec3 operator+ (const TVec3& r) const { return TVec3(x+r.x, y+r.y, z+r.z); }
	SGE_INLINE TVec3 operator- (const TVec3& r) const { return TVec3(x-r.x, y-r.y, z-r.z); }
	SGE_INLINE TVec3 operator* (const TVec3& r) const { return TVec3(x*r.x, y*r.y, z*r.z); } // non-uniform scale operator
	SGE_INLINE TVec3 operator/ (const TVec3& r) const { return TVec3(x/r.x, y/r.y, z/r.z); }

	SGE_INLINE TVec3 operator+ (const T& s) const { return TVec3(x+s, y+s, z+s); }
	SGE_INLINE TVec3 operator- (const T& s) const { return TVec3(x-s, y-s, z-s); }
	SGE_INLINE TVec3 operator* (const T& s) const { return TVec3(x*s, y*s, z*s); }
	SGE_INLINE TVec3 operator/ (const T& s) const { return TVec3(x/s, y/s, z/s); }

	SGE_INLINE void operator+= (const TVec3& r) { x+=r.x; y+=r.y; z+=r.z; }
	SGE_INLINE void operator-= (const TVec3& r) { x-=r.x; y-=r.y; z-=r.z; }
	SGE_INLINE void operator*= (const TVec3& r) { x*=r.x; y*=r.y; z*=r.z; }
	SGE_INLINE void operator/= (const TVec3& r) { x/=r.x; y/=r.y; z/=r.z; }

	SGE_INLINE void operator+= (const T& s) { x+=s; y+=s; z+=s; }
	SGE_INLINE void operator-= (const T& s) { x-=s; y-=s; z-=s; }
	SGE_INLINE void operator*= (const T& s) { x*=s; y*=s; z*=s; }
	SGE_INLINE void operator/= (const T& s) { x/=s; y/=s; z/=s; }

	SGE_INLINE TVec3 operator-() const { return TVec3(-x, -y, -z); }

	SGE_INLINE bool operator== (const TVec3& r) const { return x == r.x && y == r.y && z == r.z; }
	SGE_INLINE bool operator!= (const TVec3& r) const { return !(this->operator==(r)); }

	SGE_INLINE TVec2<T> xy() const { return TVec2<T>(x, y); }
	SGE_INLINE TVec2<T> xz() const { return TVec2<T>(x, z); }
	SGE_INLINE TVec2<T> yz() const { return TVec2<T>(y, z); }

	SGE_INLINE T		dot(const TVec3& r)		const { return (x * r.x) + (y * r.y) + (z * r.z); }
	SGE_INLINE TVec3	cross(const TVec3& r)	const { return TVec3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x); }

	SGE_INLINE TVec3	normalize()				const { T m = magnitude(); return Math::equals0(m) ? s_zero() : (*this / m); }

	SGE_INLINE T		sqrMagnitude()			const { return dot(*this); }
	SGE_INLINE T		magnitude()				const { T m = sqrMagnitude(); return Math::equals0(m) ? T(0) : Math::sqrt(m); }
	SGE_INLINE T		lenSq()					const { return sqrMagnitude(); }
	SGE_INLINE T		len()					const { return magnitude(); }

	SGE_INLINE T angle(const TVec3& r) const;

	SGE_INLINE T radians(const TVec3& r) const { return angle(r); }
	SGE_INLINE T degrees(const TVec3& r) const { return Math::degrees(radians(r)); }

	SGE_INLINE TVec3 project(const TVec3& r) const;
	SGE_INLINE TVec3 reject(const TVec3& r)  const;
	SGE_INLINE TVec3 reflect(const TVec3& r) const;

	SGE_INLINE TVec3 lerp(const TVec3& to, const T& t) const;
	SGE_INLINE TVec3 slerp(const TVec3& to_, const T& t) const; // spherical linear interpolatation: constant in velocity
	SGE_INLINE TVec3 nlerp(const TVec3& to, const T& t) const { return lerp(to, t).normalize(); } // normalize lerp: non-constant in velocity

	SGE_INLINE			T* ptr()		{ return data; }
	SGE_INLINE const	T* ptr() const	{ return data; }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {})", x, y, z);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const TVec3& v) {
		o << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return o;
	}
};

template<>
struct TVec3<int> {
	using ElementType = int;

	static const size_t kElementCount = 3;

	union {
		struct { ElementType x, y, z; };
		ElementType data[kElementCount];
	};

	SGE_INLINE TVec3() = default;
	SGE_INLINE TVec3(const ElementType& x_, const ElementType& y_, const ElementType& z_)
		: x(x_), y(y_), z(z_) {}
};

template<>
struct TVec3<unsigned int> {
	using ElementType = unsigned int;

	static const size_t kElementCount = 3;

	union {
		struct { ElementType x, y, z; };
		ElementType data[kElementCount];
	};

	SGE_INLINE TVec3() = default;
	SGE_INLINE TVec3(const ElementType& x_, const ElementType& y_, const ElementType& z_)
		: x(x_), y(y_), z(z_) {}
};

using vec3f		= TVec3<float>;
using vec3d		= TVec3<double>;
using vec3i		= TVec3<int>;
using vec3ui	= TVec3<unsigned int>;

SGE_FORMATTER_T(typename T, TVec3<T>)

#if 0
#pragma mark ------------------- instance functions -------------------
#endif
template<typename T> SGE_INLINE
bool TVec3<T>::equals(const TVec3& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon);
}

template<typename T> SGE_INLINE
bool TVec3<T>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon);
}

template<typename T> SGE_INLINE
T TVec3<T>::angle(const TVec3& r) const {
	// Finding the angle between the two vectors would be expensive
	T sqMagL = sqrMagnitude();
	T sqMagR = r.sqrMagnitude();

	if (Math::equals0(sqMagL) || Math::equals0(sqMagR)) {
		return T(0);
	}

	T cosTheta = dot(r) / Math::sqrt(sqMagL * sqMagR);
	return Math::acos(cosTheta); // radians
}

template<typename T> SGE_INLINE
TVec3<T> TVec3<T>::project(const TVec3& r) const {
/*
	projection = project *this onto r

	*this is considered as hypotenuse
	using ||hypo|| = ||hypotenuse|| = *this
	||hypo|| * cos(theta) = ||projection||

	cos(theta) = dot(r) / ||hypo|| / ||r||

	projection	= r.normalize() * ||projection||
				= (r*||r||) * (||hypo||  * cos(theta))
				= r * (||r|| * ||hypo|| * ( dot(r) / ||hypo|| / ||r|| )
				= r * (dot(r) / ||r|| / ||r||)

	let 'factor'= dot(r) / sqMagR
*/
	T sqMagR = r.sqrMagnitude();
	if (Math::equals0(sqMagR)) {
		return s_zero();
	}
	T factor = dot(r) / sqMagR;
	return r * factor;
}

template<typename T> SGE_INLINE
TVec3<T> TVec3<T>::reject(const TVec3& r) const {
/*
	rejection is perpendicular to projection
		/!
	   / ! rejection
	  /  !
	  ----
	projection
*/
	return *this - project(r);
}

template<typename T> SGE_INLINE
TVec3<T> TVec3<T>::reflect(const TVec3& r) const {
	// bounce-like reflect
	TVec3 proj = project(r);
	if (proj.equals0()) {
		return s_zero();
	}

	TVec3 proj2 = proj + proj;
	return *this - proj2;
}

template<typename T> SGE_INLINE
TVec3<T> TVec3<T>::lerp(const TVec3& to, const T& t) const {
//	linear interpolatation
//		return *this*(1.0f-t) + to*t;
//		return *this + (to - *this)*t;
//		return TVec3(x + ( (to.x-x)*t ), y + ( (to.y-y)*t ), z + ( (to.z-z)*t ));

	return TVec3(Math::lerp(x, to.x, t),
		Math::lerp(y, to.y, t),
		Math::lerp(z, to.z, t));
}

// spherical linear interpolatation: constant in velocity
template<typename T> SGE_INLINE
TVec3<T> TVec3<T>::slerp(const TVec3& to_, const T& t) const {
	// FYI: https://gabormakesgames.com/blog_vectors_interpolate.html
	// when t is close to 0, slerp will yield unexpected results
	if (Math::equals0(Math::abs(t - T(0.01)))) {
		// Linear interpolation
		return lerp(to_, t);
	}

	TVec3 from = this->normalize();
	TVec3 to = to_.normalize();

	T theta = from.radians(to);
	T sinTheta = Math::sin(theta);

	T a = Math::sin((T(1) - t) * theta) / sinTheta;
	T b = Math::sin(t * theta) / sinTheta;
	return (from * a) + (to * b);
}

}
