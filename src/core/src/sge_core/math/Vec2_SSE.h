#pragma once

#include "Vec2_Basic.h"

namespace sge {

template<class T, class M>
struct Vec2_SSE_Data {
	sgeTuple_InterfaceFunctions_Impl(Vec2_SSE_Data, T, 2)
	using RegisterType = M;

	union {
		struct { T x, y; };
		T data[kElementCount];
		M _m;
	};

	SGE_INLINE explicit constexpr Vec2_SSE_Data(T x_, T y_) : x(x_), y(y_) {
		SGE_STATIC_ASSERT(sizeof(Vec2_SSE_Data) == sizeof(T) * 2);
	}
	SGE_INLINE constexpr Vec2_SSE_Data(const Tuple2<T>& v) : x(x_), y(y_) {}

	SGE_INLINE void set(const Tuple2<T>& v)		  { set(v.x, v.y);  }
	SGE_INLINE void set(T x_, T y_)				  { x = x_; y = y_; }
	SGE_INLINE void setAll(const T& v)			  { set(v,v); }
};

using Vec2d_SSE_Data = Vec2_SSE_Data<double, __m128d>; // SSE

template<class T> struct Vec2_SSE_Select  { using Data = Vec2_Basic_Data<T>; };
template<> struct Vec2_SSE_Select<double> { using Data = Vec2d_SSE_Data; };

template<class T> using Vec2_SSE = Vec2_Basic<T, typename Vec2_SSE_Select<T>::Data >;

using Vec2f_SSE = Vec2_SSE< float>;
using Vec2d_SSE = Vec2_SSE<double>;

SGE_FORMATTER_T(class T, Vec2_SSE<T>)

SGE_INLINE constexpr Vec2d_SSE Vec2_SSE_make(__m128d m) { Vec2d_SSE o; o._m = m; return o; }

#if defined(SGE_CPU_ENDIAN_LITTLE)
template<> SGE_INLINE void Vec2d_SSE::set(double x_, double y_) { _m = _mm_set_pd(y_, x_); }

#elif defined(SGE_CPU_ENDIAN_BIG)
template<> SGE_INLINE void Vec2d_SSE::set(double x_, double y_) { _m = _mm_set_pd(x_, y_); }

#else
	#error
#endif

template<> SGE_INLINE Vec2d_SSE Vec2d_SSE::operator + (const Vec2& r) const { return Vec2_SSE_make(_mm_add_pd(_m, r._m)); }
template<> SGE_INLINE Vec2d_SSE Vec2d_SSE::operator - (const Vec2& r) const { return Vec2_SSE_make(_mm_sub_pd(_m, r._m)); }
template<> SGE_INLINE Vec2d_SSE Vec2d_SSE::operator * (const Vec2& r) const { return Vec2_SSE_make(_mm_mul_pd(_m, r._m)); }
template<> SGE_INLINE Vec2d_SSE Vec2d_SSE::operator / (const Vec2& r) const { return Vec2_SSE_make(_mm_div_pd(_m, r._m)); }

}