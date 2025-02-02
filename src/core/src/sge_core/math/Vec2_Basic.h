#pragma once

#include "Tuple2.h"

namespace sge {

template<class T> using Vec2_Basic_Data = Tuple2<T>;

template<class T, class DATA = Vec2_Basic_Data<T> >
struct Vec2_Basic : public DATA {
	using Vec2 = Vec2_Basic;

	using ElementType	= typename DATA::ElementType;
	using Scalar		= T;

	using DATA::x;
	using DATA::y;
	using DATA::data;
	using DATA::kElementCount;

	SGE_INLINE static Vec2 s_zero()		{ return Vec2(0,0); }
	SGE_INLINE static Vec2 s_one()		{ return Vec2(1,1); }

	SGE_INLINE static Vec2 s_up()		{ return Vec2( 0, 1); }
	SGE_INLINE static Vec2 s_down()		{ return Vec2( 0,-1); }
	SGE_INLINE static Vec2 s_right()	{ return Vec2( 1, 0); }
	SGE_INLINE static Vec2 s_left()		{ return Vec2(-1, 0); }

	SGE_INLINE static Vec2 s_inf()		{ auto f = Math::inf<T>(); return Vec2(f,f); }

	Vec2() = default;
	constexpr Vec2(const Tuple2<T> & v) { set(v); }
	constexpr Vec2(const T& x_, const T& y_) { set(x_, y_); }

	SGE_INLINE void set(const Tuple2<T> & v)		{ DATA::set(v); }
	SGE_INLINE void set(const T& x_, const T& y_)	{ set(Tuple2<T>(x_, y_)); }

	SGE_INLINE bool equals(const Vec2& r, const T& epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(              const T& epsilon = Math::epsilon<T>()) const;

	SGE_INLINE void setAll(const T& v) { set(v,v); }
	SGE_INLINE bool isAll (const T& v) { return equals(Vec2(v,v)); }

	SGE_INLINE Vec2 operator-() const { return Vec2(-x, -y); }

	SGE_INLINE Vec2 operator+(const Vec2& r) const { return Vec2(x + r.x, y + r.y); }
	SGE_INLINE Vec2 operator-(const Vec2& r) const { return Vec2(x - r.x, y - r.y); }
	SGE_INLINE Vec2 operator*(const Vec2& r) const { return Vec2(x * r.x, y * r.y); }
	SGE_INLINE Vec2 operator/(const Vec2& r) const { return Vec2(x / r.x, y / r.y); }

	SGE_INLINE Vec2 operator+(const Scalar& s) const { return Vec2(x + s, y + s); }
	SGE_INLINE Vec2 operator-(const Scalar& s) const { return Vec2(x - s, y - s); }
	SGE_INLINE Vec2 operator*(const Scalar& s) const { return Vec2(x * s, y * s); }
	SGE_INLINE Vec2 operator/(const Scalar& s) const { return Vec2(x / s, y / s); }

	SGE_INLINE void operator+= (const Vec2& r) { x += r.x; y += r.y; }
	SGE_INLINE void operator-= (const Vec2& r) { x -= r.x; y -= r.y; }
	SGE_INLINE void operator*= (const Vec2& r) { x *= r.x; y *= r.y; }
	SGE_INLINE void operator/= (const Vec2& r) { x /= r.x; y /= r.y; }

	SGE_INLINE void operator+= (const Scalar& s) { x += s; y += s; }
	SGE_INLINE void operator-= (const Scalar& s) { x -= s; y -= s; }
	SGE_INLINE void operator*= (const Scalar& s) { x *= s; y *= s; }
	SGE_INLINE void operator/= (const Scalar& s) { x /= s; y /= s; }

	SGE_INLINE			T& operator[](int i)		{ return data[i]; }
	SGE_INLINE	const	T& operator[](int i) const	{ return data[i]; }

	bool operator==(const Vec2& r) const { return x == r.x && y == r.y; }
	bool operator!=(const Vec2& r) const { return x != r.x || y != r.y; }

	SGE_NODISCARD SGE_INLINE T		magnitude		() const	{ return Math::sqrt (sqrMagnitude()); }
	SGE_NODISCARD SGE_INLINE T		sqrMagnitude	() const	{ return x*x + y*y; }

	SGE_NODISCARD SGE_INLINE T		length			() const	{ return magnitude(); }
	SGE_NODISCARD SGE_INLINE T		sqrLength		() const	{ return sqrMagnitude(); }

	SGE_NODISCARD SGE_INLINE T		distance		(const Vec2& r) const { return (*this - r).length();    }
	SGE_NODISCARD SGE_INLINE T		sqrDistance		(const Vec2& r) const { return (*this - r).sqrLength(); }

	SGE_INLINE constexpr Vec2		yx()  const { return Vec2(y,x); }

	Tuple2<T> toTuple()  const { return Tuple2<T>(x,y); }
	operator Tuple2<T>() const { return toTuple(); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {})", x, y);
	}

	template<class R, class DATA>
	static Vec2 s_cast(const Vec2_Basic<R, DATA>& r) {
		return Vec2(static_cast<T>(r.x), static_cast<T>(r.y));
	}
};

using Vec2f_Basic = Vec2_Basic<float>;
using Vec2d_Basic = Vec2_Basic<double>;

SGE_FORMATTER_T( SGE_ARGS(class T, class DATA), Vec2_Basic< SGE_ARGS(T, DATA) >)

template<class T, class DATA> SGE_INLINE
bool Vec2_Basic<T, DATA>::equals(const Vec2& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon);
}

template<class T, class DATA> SGE_INLINE
bool Vec2_Basic<T, DATA>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon);
}

namespace Math {

template<class T, class DATA> SGE_INLINE
void sincos(const Vec2_Basic<T, DATA>& th, Vec2_Basic<T, DATA>& outSin, Vec2_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
}

template<class T, class DATA> SGE_INLINE
Vec2_Basic<T, DATA> min(const Vec2_Basic<T, DATA>& a, const Vec2_Basic<T, DATA>& b) {
	return Vec2_Basic<T, DATA>(	Math::min(a.x, b.x),
								Math::min(a.y, b.y));
}

template<class T, class DATA> SGE_INLINE
Vec2_Basic<T, DATA> max(const Vec2_Basic<T, DATA>& a, const Vec2_Basic<T, DATA>& b) {
	return Vec2_Basic<T, DATA>(	Math::max(a.x, b.x),
								Math::max(a.y, b.y));
}

template<class T, class DATA> SGE_INLINE
Vec2_Basic<T, DATA> clamp(const Vec2_Basic<T, DATA>& v, const Vec2_Basic<T, DATA>& a, const Vec2_Basic<T, DATA>& b) {
	return Vec2_Basic<T, DATA>(	Math::clamp(v.x, a.x, b.x),
								Math::clamp(v.y, a.y, b.y));
}

}} // namespace sge/Math