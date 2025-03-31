#pragma once

#include "Tuple2.h"

namespace sge {

template<class T>
struct Tuple3 {
	sgeTuple_InterfaceFunctions_Impl(Tuple3, T, 3)
	union {
		struct { T x, y, z; };
		T data[kElementCount];
	};

	SGE_INLINE constexpr explicit Tuple3(T v)			   : x(v), y(v), z(v) {}
	SGE_INLINE constexpr explicit Tuple3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}

	SGE_INLINE constexpr void set(const Tuple3<T>& v) { *this = v; }
	SGE_INLINE constexpr void set(T x_, T y_, T z_) {
		x = x_; y = y_; z = z_;
	}
	SGE_INLINE constexpr void setAll(const T& v)  { set(v,v,v); }
	SGE_INLINE constexpr void setToDefaultValue() { setAll(0); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {})", x, y, z);
	}
};

using Tuple3f = Tuple3<float>;
using Tuple3d = Tuple3<double>;
using Tuple3i = Tuple3<i32>;

SGE_FORMATTER_T(class T, Tuple3<T>)

}