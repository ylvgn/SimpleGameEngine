#pragma once

namespace sge {

template<typename T>
struct MyTuple2 {

	using ElementType = T;

	static constexpr size_t kElementCount = 2;

	union {
		struct { T x, y; };
		T data[kElementCount];
	};

	constexpr MyTuple2() = default;
	constexpr MyTuple2(const T& x_, const T& y_)
		: x(x_), y(y_) {}

	SGE_INLINE void set(const MyTuple2<T>& v) { *this = v; }
	SGE_INLINE void set(const T& x_, const T& y_) {
		x = x_; y = y_;
	}

	SGE_INLINE void setAll(const T& v) { set(v,v); }
};

using MyTuple2f = MyTuple2<float>;
using MyTuple2d = MyTuple2<double>;
using MyTuple2i = MyTuple2<int>;

}