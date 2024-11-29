#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/log/Log.h>
#include <sge_core/base/Reflection.h>
#include "Math.h"

namespace sge {

template<class T>
struct Tuple2 {
	using ElementType = T;

	static const size_t kElementCount = 2;

	union {
		struct { T x, y; };
		T data[kElementCount];
	};

	Tuple2() = default;
	constexpr Tuple2(const T& x_, const T& y_) : x(x_), y(y_) {}

	constexpr void set(const Tuple2<T>& v) { *this = v; }
	constexpr void set(const T& x_, const T& y_) {
		x = x_; y = y_;
	}

	constexpr bool setAll(const T& v) { set(v,v); }

	SGE_INLINE			T& operator[](int i)		{ return data[i]; }
	SGE_INLINE	const	T& operator[](int i) const	{ return data[i]; }
};

using Tuple2i = Tuple2<i32>;
using Tuple2f = Tuple2<float>;
using Tuple2d = Tuple2<double>;

} // namespace sge