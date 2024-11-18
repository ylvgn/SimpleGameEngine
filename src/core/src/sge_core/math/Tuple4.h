#pragma once

namespace sge {

template<class T>
struct Tuple4 {
	using ElementType = T;

	static const size_t kElementCount = 4;

	union {
		struct { T x, y, z, w; };
		T data[kElementCount];
	};

	Tuple4() = default;
	constexpr Tuple4(const T& x_, const T& y_, const T& z_, const T& w_)
		: x(x_), y(y_), z(z_), w(w_) {}

	constexpr void set(const Tuple4<T>& v) { *this = v; }
	constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) {
		x = x_; y = y_; z = z_; w = w_;
	}

	constexpr bool setAll(const T& v) { set(v,v,v,v); }

	SGE_INLINE			T& operator[](int i)		{ return data[i]; }
	SGE_INLINE	const	T& operator[](int i) const	{ return data[i]; }
};

using Tuple4i = Tuple4<i32>;
using Tuple4f = Tuple4<float>;
using Tuple4d = Tuple4<double>;

}