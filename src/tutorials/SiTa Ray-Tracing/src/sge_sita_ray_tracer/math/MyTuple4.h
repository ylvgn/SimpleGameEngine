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

	constexpr void set(const MyTuple4<T>& v) { *this = v; }
	constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) {
		x = x_; y = y_; z = z_; w = w_;
	}

	constexpr void setAll(const T& v) { set(v,v,v,v); }

	constexpr MyTuple4(const sge::Tuple4<T>& v) : MyTuple4(v.x, v.y, v.z, v.w) {}
	constexpr void set(const sge::Tuple4<T>& v) { *this = v; }
	constexpr void operator= (const sge::Tuple4<T>& v) { set(v.x, v.y, v.z, v.w); }
};

using MyTuple4f = MyTuple4<float>;
using MyTuple4d = MyTuple4<double>;
using MyTuple4i = MyTuple4<int>;

}