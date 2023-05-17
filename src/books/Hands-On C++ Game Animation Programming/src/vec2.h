#pragma once

namespace sge {

template<typename T>
struct TVec2 {

	union {
		struct { T x, y; };
		T v[2];
	};

	inline TVec2() : x(0), y(0) {}
	inline TVec2(const T& x_, const T& y_) : x(x_), y(y_) {}

	inline			T& operator[](int i)		{ return v[i]; }
	inline const	T& operator[](int i) const	{ return v[i]; }

	inline bool equals(const TVec2& r, const T& epsilon = Math::epsilon<T>()) const;
	inline bool equals0(               const T& epsilon = Math::epsilon<T>()) const;

	inline TVec2 operator+ (const TVec2& r) const { return TVec2(x+r.x, y+r.y); }
	inline TVec2 operator- (const TVec2& r) const { return TVec2(x-r.x, y-r.y); }
	inline TVec2 operator* (const TVec2& r) const { return TVec2(x*r.x, y*r.y); }
	inline TVec2 operator/ (const TVec2& r) const { return TVec2(x/r.x, y/r.y); }

	inline void operator+= (const TVec2& r) { x += r.x; y += r.y; }
	inline void operator-= (const TVec2& r) { x -= r.x; y -= r.y; }
	inline void operator*= (const TVec2& r) { x *= r.x; y *= r.y; }
	inline void operator/= (const TVec2& r) { x /= r.x; y /= r.y; }

	inline TVec2 operator* (const T& s) const	{ return TVec2(x*s, y*s); }
	inline TVec2 operator/ (const T& s) const	{ return TVec2(x/s, y/s); }
	inline void operator*= (const T& s)			{ x *= s; y *= s; }
	inline void operator/= (const T& s)			{ x /= s; y /= s; }

	inline TVec2 operator-()			const	{ return TVec2(-x, -y); }

	inline bool operator== (const TVec2& r) const { return x == r.x && y == r.y; }
	inline bool operator!= (const TVec2& r) const { return !(this->operator==(r)); }
};

template<typename T>
inline bool TVec2<T>::equals(const TVec2<T>& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon) && Math::equals(y, r.y, epsilon);
}

template<typename T>
inline bool TVec2<T>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon) && Math::equals0(y, epsilon);
}

template<>
struct TVec2<int> {
	using T = int;
	using This = TVec2<T>;
	union {
		struct { T x, y; };
		T v[2];
	};

	inline TVec2(const T& x_, const T& y_) : x(x_), y(y_) { }
};

template<>
struct TVec2<unsigned int> {
	using T = unsigned int;
	using This = TVec2<T>;
	union {
		struct { T x, y; };
		T v[2];
	};

	inline TVec2(const T& x_, const T& y_) : x(x_), y(y_) { }
};

using vec2		= TVec2<float>;
using ivec2		= TVec2<int>;
using uivec2	= TVec2<unsigned int>;

}