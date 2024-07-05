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

	SGE_INLINE void set(const MyTuple3<T>& v) { *this = v; }
	SGE_INLINE void set(const T& x_, const T& y_, const T& z_) {
		x = x_; y = y_; z = z_;
	}

	SGE_INLINE void setAll(const T& v) { set(v,v,v); }
};

using MyTuple3f = MyTuple3<float>;
using MyTuple3d = MyTuple3<double>;
using MyTuple3i = MyTuple3<int>;

}