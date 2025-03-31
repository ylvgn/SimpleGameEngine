#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/log/Log.h>
#include <sge_core/base/Reflection.h>
#include "Math.h"

#define sgeTuple_InterfaceFunctions_Impl(SGE_T, ELEMENT_TYPE, COUNT) \
private: \
	using This = SGE_T; \
public: \
	using ElementType = ELEMENT_TYPE; \
	static const size_t kElementCount = COUNT; \
	\
	SGE_INLINE explicit	constexpr This() = default; \
	\
	SGE_INLINE			ElementType& operator[]	(int i)			{ return at(i); } \
	SGE_INLINE	const	ElementType& operator[]	(int i)	const	{ return at(i); } \
	\
	SGE_INLINE			ElementType& at			(int i)			{ s_checkBound(i); return data[i]; } \
	SGE_INLINE	const	ElementType& at			(int i)	const	{ s_checkBound(i); return data[i]; } \
	\
	SGE_INLINE			ElementType& unsafe_at	(int i)			{ return data[i]; } \
	SGE_INLINE	const	ElementType& unsafe_at	(int i)	const	{ return data[i]; } \
	\
	SGE_INLINE			Span<		ElementType> span()			{ return Span<		ElementType>(&x, kElementCount); } \
	SGE_INLINE			Span<const	ElementType> span() const	{ return Span<const ElementType>(&x, kElementCount); } \
	\
	template<class V> SGE_INLINE constexpr void setByCast(const V& v) { *this = s_cast(v); } \
private: \
	SGE_INLINE static constexpr bool s_inBound		(int i)		{ return i >= 0 && i < kElementCount; } \
	SGE_INLINE static constexpr void s_checkBound	(int i)		{ if (!s_inBound(i)) throw std::out_of_range(__FILE__); } \
public: \
//-----

namespace sge {

template<class T>
struct Tuple2 {
	sgeTuple_InterfaceFunctions_Impl(Tuple2, T, 2)
	union {
		struct { T x, y; };
		T data[kElementCount];
	};

	SGE_INLINE constexpr explicit Tuple2(T v)		 : x(v), y(v) {}
	SGE_INLINE constexpr explicit Tuple2(T x_, T y_) : x(x_), y(y_) {}

	SGE_INLINE constexpr void set(const Tuple2<T>& v) { *this = v; }
	SGE_INLINE constexpr void set(T x_, T y_) {
		x = x_; y = y_;
	}
	SGE_INLINE constexpr void setAll(const T& v)  { set(v,v); }
	SGE_INLINE constexpr void setToDefaultValue() { setAll(0); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {})", x, y);
	}
};

using Tuple2f = Tuple2<float>;
using Tuple2d = Tuple2<double>;
using Tuple2i = Tuple2<i32>;

SGE_FORMATTER_T(class T, Tuple2<T>)

}