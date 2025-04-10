#pragma once
#include "Vec4_Basic.h"

namespace sge {

template<class T, class M>
struct Vec4_SSE_Data {
	sgeTuple_InterfaceFunctions_Impl(Vec4_SSE_Data, T, 4)
	using RegisterType = M;

	union {
		struct { T x, y, z, w; };
		T data[kElementCount];
		M _m;
	};

	SGE_INLINE explicit constexpr Vec4_SSE_Data(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {
		SGE_STATIC_ASSERT(sizeof(Vec4_SSE_Data) == sizeof(T) * 4);
	}
	SGE_INLINE constexpr Vec4_SSE_Data(const Tuple4<T>& v) : x(x_), y(y_), z(z_), w(w_) {}

	SGE_INLINE void set(T x_, T y_, T z_, T w_) { x = x_; y = y_; z = z_; w = w_; }
	SGE_INLINE void set(const Tuple4<T>& v)		{ set(v.x, v.y, v.z, v.w); }
	SGE_INLINE void setAll(const T& v)			{ set(v,v,v,v); }
};

using Vec4f_SSE_Data = Vec4_SSE_Data< float,  __m128>; // SSE
using Vec4d_SSE_Data = Vec4_SSE_Data<double, __m256d>; // AVX

template<class T> struct Vec4_SSE_Select  { using Data = Vec4_Basic_Data<T>; };
template<> struct Vec4_SSE_Select< float> { using Data = Vec4f_SSE_Data; };
template<> struct Vec4_SSE_Select<double> { using Data = Vec4d_SSE_Data; };

template<class T> using Vec4_SSE = Vec4_Basic<T, typename Vec4_SSE_Select<T>::Data >;

using Vec4f_SSE = Vec4_SSE< float>;
using Vec4d_SSE = Vec4_SSE<double>;

SGE_FORMATTER_T(class T, Vec4_SSE<T>)

SGE_INLINE constexpr Vec4f_SSE Vec4_SSE_make( __m128 m) { Vec4f_SSE o; o._m = m; return o; }
SGE_INLINE constexpr Vec4d_SSE Vec4_SSE_make(__m256d m) { Vec4d_SSE o; o._m = m; return o; }

#if defined(SGE_CPU_ENDIAN_LITTLE)
template<> SGE_INLINE void Vec4f_SSE::set( float x_,  float y_,  float z_,  float w_) { _m =	_mm_set_ps(w_, z_, y_, x_); }
template<> SGE_INLINE void Vec4d_SSE::set(double x_, double y_, double z_, double w_) { _m = _mm256_set_pd(w_, z_, y_, x_); }

#elif defined(SGE_CPU_ENDIAN_BIG)
template<> SGE_INLINE void Vec4f_SSE::set( float x_,  float y_,  float z_,  float w_) { _m =	_mm_set_ps(x_, y_, z_, w_); }
template<> SGE_INLINE void Vec4d_SSE::set(double x_, double y_, double z_, double w_) { _m = _mm256_set_pd(x_, y_, z_, w_); }

#else
	#error
#endif

template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator + (const Vec4& r) const { return Vec4_SSE_make(   _mm_add_ps(_m, r._m)); }
template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator - (const Vec4& r) const { return Vec4_SSE_make(   _mm_sub_ps(_m, r._m)); }
template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator * (const Vec4& r) const { return Vec4_SSE_make(   _mm_mul_ps(_m, r._m)); }
template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator / (const Vec4& r) const { return Vec4_SSE_make(   _mm_div_ps(_m, r._m)); }

template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator + (const Vec4& r) const { return Vec4_SSE_make(_mm256_add_pd(_m, r._m)); }
template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator - (const Vec4& r) const { return Vec4_SSE_make(_mm256_sub_pd(_m, r._m)); }
template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator * (const Vec4& r) const { return Vec4_SSE_make(_mm256_mul_pd(_m, r._m)); }
template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator / (const Vec4& r) const { return Vec4_SSE_make(_mm256_div_pd(_m, r._m)); }

template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator + (		 float s) const { return Vec4_SSE_make(   _mm_add_ps(_m,    _mm_set1_ps(s))); }
template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator - (		 float s) const { return Vec4_SSE_make(   _mm_sub_ps(_m,    _mm_set1_ps(s))); }
template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator * (		 float s) const { return Vec4_SSE_make(   _mm_mul_ps(_m,    _mm_set1_ps(s))); }
template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator / (		 float s) const { return Vec4_SSE_make(   _mm_div_ps(_m,    _mm_set1_ps(s))); }

template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator + (		double s) const { return Vec4_SSE_make(_mm256_add_pd(_m, _mm256_set1_pd(s))); }
template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator - (		double s) const { return Vec4_SSE_make(_mm256_sub_pd(_m, _mm256_set1_pd(s))); }
template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator * (		double s) const { return Vec4_SSE_make(_mm256_mul_pd(_m, _mm256_set1_pd(s))); }
template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator / (		double s) const { return Vec4_SSE_make(_mm256_div_pd(_m, _mm256_set1_pd(s))); }

}