#pragma once

namespace sge {

template<typename T>
struct MyTuple3 {
	
	using ElementType = T;

	static constexpr size_t kElementCount = 3;

	union {
		struct { T x, y, z; };
		T data[kElementCount];
	};

	constexpr MyTuple3() = default;
	constexpr MyTuple3(const T& x_, const T& y_, const T& z_)
		: x(x_), y(y_), z(z_) {}

	constexpr void set(const MyTuple3<T>& v) { *this = v; }
	constexpr void set(const T& x_, const T& y_, const T& z_) {
		x = x_; y = y_; z = z_;
	}

	constexpr void setAll(const T& v) { set(v,v,v); }

	constexpr MyTuple3(const sge::Tuple3<T>& v) : MyTuple3(v.x, v.y, v.z) {}
	constexpr void set(const sge::Tuple3<T>& v) { *this = v; }
	void operator= (const sge::Tuple3<T>& v) { set(v.x, v.y, v.z); }
};

using MyTuple3f = MyTuple3<float>;
using MyTuple3d = MyTuple3<double>;
using MyTuple3i = MyTuple3<int>;

}