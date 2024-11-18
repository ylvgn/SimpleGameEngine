#pragma once

namespace sge {

template<class T>
struct Tuple3 {
	using ElementType = T;

	static const size_t kElementCount = 3;

	union {
		struct { T x, y, z; };
		T data[kElementCount];
	};

	Tuple3() = default;
	constexpr Tuple3(const T& x_, const T& y_, const T& z_)
		: x(x_), y(y_), z(z_) {}

	constexpr void set(const Tuple3<T>& v) { *this = v; }
	constexpr void set(const T& x_, const T& y_, const T& z_) {
		x = x_; y = y_; z = z_;
	}

	constexpr bool setAll(const T& v) { set(v,v,v); }

	SGE_INLINE			T& operator[](int i)		{ return data[i]; }
	SGE_INLINE	const	T& operator[](int i) const	{ return data[i]; }
};

using Tuple3i = Tuple3<i32>;
using Tuple3f = Tuple3<float>;
using Tuple3d = Tuple3<double>;

}