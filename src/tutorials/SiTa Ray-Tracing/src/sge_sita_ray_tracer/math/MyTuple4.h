#pragma once

namespace sge {

template<typename T>
struct MyTuple4 {
	using ElementType = T;

	static constexpr size_t kElementCount = 4;

	union {
		struct { T x, y, z, w; };
		T data[kElementCount];
	};

	constexpr MyTuple4() = default;
	constexpr MyTuple4(const T& x_, const T& y_, const T& z_, const T& w_)
		: x(x_), y(y_), z(z_), w(w_) {}

	SGE_INLINE void set(const MyTuple4<T>& v) { *this = v; }
	SGE_INLINE void set(const T& x_, const T& y_, const T& z_, const T& w_)	{
		x = x_; y = y_; z = z_; w = w_;
	}

	SGE_INLINE void setAll(const T& v) { set(v,v,v,v); }
};

using MyTuple4f = MyTuple4<float>;
using MyTuple4d = MyTuple4<double>;
using MyTuple4i = MyTuple4<int>;

}