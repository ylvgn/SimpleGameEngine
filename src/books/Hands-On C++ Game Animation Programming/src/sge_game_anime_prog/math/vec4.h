#pragma once

#include "vec3.h"

namespace sge {

template<typename T>
struct TVec4 {

	using vec2 = TVec2<T>;
	using vec3 = TVec3<T>;

	static const size_t kElementCount = 4;

	union {
		struct { T x, y, z, w; };
		T data[kElementCount];
	};

	SGE_INLINE TVec4() = default;
	SGE_INLINE TVec4(const T& x_, const T& y_, const T& z_, const T& w_) : x(x_), y(y_), z(z_), w(w_) {}
	SGE_INLINE TVec4(const vec2& v2, const T& z_, const T& w_) : x(v2.x), y(v2.y), z(z_), w(w_) {}
	SGE_INLINE TVec4(const vec3& v3, const T& w_) : x(v3.x), y(v3.y), z(v3.z), w(w_) {}

	SGE_INLINE void set(const T& x_, const T& y_, const T& z_, const T& w_) { x = x_; y = y_; z = z_; w = w_; }

	SGE_INLINE static TVec4 s_zero()	{ return TVec4(0, 0, 0, 0); }
	SGE_INLINE static TVec4 s_one()		{ return TVec4(1, 1, 1, 1); }

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const	{ SGE_ASSERT(i < kElementCount); return data[i]; }

	SGE_INLINE bool equals(const TVec4& r, const T& epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(const T& epsilon = Math::epsilon<T>()) const;

	SGE_INLINE TVec4 operator+ (const TVec4& r) const { return TVec4(x+r.x, y+r.y, z+r.z, w+r.w); }
	SGE_INLINE TVec4 operator- (const TVec4& r) const { return TVec4(x-r.x, y-r.y, z-r.z, w-r.w); }
	SGE_INLINE TVec4 operator* (const TVec4& r) const { return TVec4(x*r.x, y*r.y, z*r.z, w*r.w); } // non-uniform scale operator
	SGE_INLINE TVec4 operator/ (const TVec4& r) const { return TVec4(x/r.x, y/r.y, z/r.z, w/r.w); }

	SGE_INLINE TVec4 operator+ (const T& s)		const { return TVec4(x+s, y+s, z+s, w+s); }
	SGE_INLINE TVec4 operator- (const T& s)		const { return TVec4(x-s, y-s, z-s, w-s); }
	SGE_INLINE TVec4 operator* (const T& s)		const { return TVec4(x*s, y*s, z*s, w*s); }
	SGE_INLINE TVec4 operator/ (const T& s)		const { return TVec4(x/s, y/s, z/s, w/s); }

	SGE_INLINE void operator+= (const TVec4& r) { x+=r.x; y+=r.y; z+=r.z; w+=r.w; }
	SGE_INLINE void operator-= (const TVec4& r) { x-=r.x; y-=r.y; z-=r.z; w-=r.w; }
	SGE_INLINE void operator*= (const TVec4& r) { x*=r.x; y*=r.y; z*=r.z; w*=r.w; }
	SGE_INLINE void operator/= (const TVec4& r) { x/=r.x; y/=r.y; z/=r.z; w/=r.w; }

	SGE_INLINE void operator+= (const T& s) { x+=s; y+=s; z+=s; w+=s; }
	SGE_INLINE void operator-= (const T& s) { x-=s; y-=s; z-=s; w-=s; }
	SGE_INLINE void operator*= (const T& s) { x*=s; y*=s; z*=s; w*=s; }
	SGE_INLINE void operator/= (const T& s) { x/=s; y/=s; z/=s; w/=s; }

	SGE_INLINE TVec4 operator-() const { return TVec4(-x, -y, -z, -w); }

	SGE_INLINE bool operator== (const TVec4& r) const	{ return x == r.x && y == r.y && z == r.z && w == r.w; }
	SGE_INLINE bool operator!= (const TVec4& r) const	{ return !(this->operator==(r)); }

	SGE_INLINE vec2 xy()		const { return vec2(x, y); }
	SGE_INLINE vec2 xz()		const { return vec2(x, z); }
	SGE_INLINE vec2 yz()		const { return vec2(y, z); }
	SGE_INLINE vec3 xyz()		const { return vec3(x, y, z); }

	SGE_INLINE T dot(const TVec4& r) const { return (x * r.x) + (y * r.y) + (z * r.z) + (w * r.w); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, z, w);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const TVec4& v) {
		o << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
		return o;
	}
};

template<>
struct TVec4<int> {
	using ElementType = int;

	static const size_t kElementCount = 4;

	union {
		struct { ElementType x, y, z, w; };
		ElementType data[kElementCount];
	};

	SGE_INLINE TVec4() = default;
	SGE_INLINE TVec4(const ElementType& x_, const ElementType& y_, const ElementType& z_, const ElementType& w_)
		: x(x_), y(y_), z(z_), w(w_) {}
};

template<>
struct TVec4<unsigned int> {
	using ElementType = unsigned int;

	static const size_t kElementCount = 4;

	union {
		struct { ElementType x, y, z, w; };
		ElementType data[kElementCount];
	};

	SGE_INLINE TVec4() = default;
	SGE_INLINE TVec4(const ElementType& x_, const ElementType& y_, const ElementType& z_, const ElementType& w_)
		: x(x_), y(y_), z(z_), w(w_) {}
};

using vec4f		= TVec4<float>;
using vec4d		= TVec4<double>;
using vec4i		= TVec4<int>;
using vec4ui	= TVec4<unsigned int>;

SGE_FORMATTER_T(typename T, TVec4<T>)

#if 0
#pragma mark ------------------- instance functions -------------------
#endif
template<class T> SGE_INLINE
bool TVec4<T>::equals(const TVec4<T>& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon)
		&& Math::equals(w, r.w, epsilon);
}

template<class T> SGE_INLINE
bool TVec4<T>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon)
		&& Math::equals0(w, epsilon);
}

}
