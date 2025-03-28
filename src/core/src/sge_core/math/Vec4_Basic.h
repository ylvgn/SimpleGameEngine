#pragma once

#include "Tuple4.h"
#include "Vec3.h"

namespace sge {

template<class T> using Vec4_Basic_Data = Tuple4<T>;

template<class T, class DATA = Vec4_Basic_Data<T> >
struct Vec4_Basic : public DATA {
public:	
	using Vec4 = Vec4_Basic;
	using Vec2 = Vec2<T>;
	using Vec3 = Vec3<T>;

	using ElementType	= typename DATA::ElementType;
	using Scalar		= T;

	using DATA::x; // require this on gcc/clang, otherwise the fullname `Base::x` is needed instead of `x`
	using DATA::y;
	using DATA::z;
	using DATA::w;
	using DATA::data;
	using DATA::kElementCount;

	SGE_INLINE static constexpr Vec4 s_zero()	{ return Vec4(0,0,0,0); }
	SGE_INLINE static constexpr Vec4 s_one()	{ return Vec4(1,1,1,1); }

	SGE_INLINE static constexpr Vec4 s_inf()	{ auto f = Math::inf<T>(); return Vec4(f,f,f,f); }

	SGE_INLINE static constexpr Vec4 s_xy01(const Vec2& v) { return Vec4(v.x, v.y, T(0), T(1)); }
	SGE_INLINE static constexpr Vec4 s_xyz1(const Vec3& v) { return Vec4(v.x, v.y, v.z, T(1)); }

	Vec4() = default;
	constexpr Vec4(const Tuple4<T>& v) { set(v); }
	constexpr Vec4(const T& x_, const T& y_, const T& z_, const T& w_) { set(x_, y_, z_, w_); }
	constexpr Vec4(const Vec3& v, const T& w_) { set(v.x, v.y, v.z, w_); }

	SGE_INLINE void set(const Tuple4<T> & v) { DATA::set(v); }
	SGE_INLINE void set(const T& x_, const T& y_, const T& z_, const T& w_) { set(Tuple4<T>(x_, y_, z_, w_)); }

	SGE_INLINE bool equals(const Vec4& r, const T& epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(              const T& epsilon = Math::epsilon<T>()) const;

	SGE_INLINE void setAll(const T& v) { set(v,v,v,v); }
	SGE_INLINE bool isAll (const T& v) { return equals(Vec4(v,v,v,v)); }

	SGE_INLINE Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }

	SGE_INLINE Vec4 operator+(const Vec4& r) const { return Vec4(x + r.x, y + r.y, z + r.z, w + r.w); }
	SGE_INLINE Vec4 operator-(const Vec4& r) const { return Vec4(x - r.x, y - r.y, z - r.z, w - r.w); }
	SGE_INLINE Vec4 operator*(const Vec4& r) const { return Vec4(x * r.x, y * r.y, z * r.z, w * r.w); }
	SGE_INLINE Vec4 operator/(const Vec4& r) const { return Vec4(x / r.x, y / r.y, z / r.z, w / r.w); }

	SGE_INLINE Vec4 operator+(const Scalar& s) const { return Vec4(x + s, y + s, z + s, w + s); }
	SGE_INLINE Vec4 operator-(const Scalar& s) const { return Vec4(x - s, y - s, z - s, w - s); }
	SGE_INLINE Vec4 operator*(const Scalar& s) const { return Vec4(x * s, y * s, z * s, w * s); }
	SGE_INLINE Vec4 operator/(const Scalar& s) const { return Vec4(x / s, y / s, z / s, w / s); }

	SGE_INLINE void operator+=(const Vec4& r) { x += r.x; y += r.y; z += r.z; w += r.w; }
	SGE_INLINE void operator-=(const Vec4& r) { x -= r.x; y -= r.y; z -= r.z; w -= r.w; }
	SGE_INLINE void operator*=(const Vec4& r) { x *= r.x; y *= r.y; z *= r.z; w *= r.w; }
	SGE_INLINE void operator/=(const Vec4& r) { x /= r.x; y /= r.y; z /= r.z; w /= r.w; }

	SGE_INLINE void operator+=(const Scalar& s) { x += s; y += s; z += s; w += s; }
	SGE_INLINE void operator-=(const Scalar& s) { x -= s; y -= s; z -= s; w -= s; }
	SGE_INLINE void operator*=(const Scalar& s) { x *= s; y *= s; z *= s; w *= s; }
	SGE_INLINE void operator/=(const Scalar& s) { x /= s; y /= s; z /= s; w /= s; }

	SGE_INLINE bool operator==(const Vec4& r) const { return x == r.x && y == r.y && z == r.z && w == r.w; }
	SGE_INLINE bool operator!=(const Vec4& r) const { return x != r.x || y != r.y || z != r.z || w != r.w; }

			T& operator[](int i)		{ return data[i]; }
	const	T& operator[](int i) const	{ return data[i]; }

	SGE_INLINE constexpr Vec2 xy()  const { return Vec2(x, y); }
	SGE_INLINE constexpr Vec2 xz()  const { return Vec2(x, z); }
	SGE_INLINE constexpr Vec2 yz()  const { return Vec2(y,z); }

	SGE_INLINE constexpr Vec3 xyz() const { return Vec3(x, y, z); }
	SGE_INLINE constexpr Vec3 xy0() const { return Vec3(x, y, 0); }
	SGE_INLINE constexpr Vec3 x0z() const { return Vec3(x,0,z); }

	Vec3 toVec3	() const { return (*this / w).xyz(); };

	Tuple4<T> toTuple()  const { return Tuple4<T>(x,y,z,w); }
	operator Tuple4<T>() const { return toTuple(); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, z, w);
	}

	template<class R, class DATA> constexpr
	static Vec4 s_cast(const Vec4_Basic<R, DATA>& r) {
		return Vec4(static_cast<T>(r.x), static_cast<T>(r.y), static_cast<T>(r.z), static_cast<T>(r.w));
	}
};

using Vec4f_Basic = Vec4_Basic<float>;
using Vec4d_Basic = Vec4_Basic<double>;

SGE_FORMATTER_T( SGE_ARGS(class T, class DATA), Vec4_Basic< SGE_ARGS(T, DATA) >)


template<class T, class DATA> SGE_INLINE
bool Vec4_Basic<T, DATA>::equals(const Vec4& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon)
		&& Math::equals(w, r.w, epsilon);
}

template<class T, class DATA> SGE_INLINE
bool Vec4_Basic<T, DATA>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon)
		&& Math::equals0(w, epsilon);
}

namespace Math {

template<class T, class DATA> SGE_INLINE
void sincos(const Vec4_Basic<T, DATA>& th, Vec4_Basic<T, DATA>& outSin, Vec4_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
	Math::sincos(th.z, outSin.z, outCos.z);
	Math::sincos(th.w, outSin.w, outCos.w);
}

template<class T, class DATA> SGE_INLINE
Vec4_Basic<T, DATA> min(const Vec4_Basic<T, DATA>& a, const Vec4_Basic<T, DATA>& b) {
	return Vec4_Basic<T, DATA>(	Math::min(a.x, b.x),
								Math::min(a.y, b.y),
								Math::min(a.z, b.z),
								Math::min(a.w, b.w));
}

template<class T, class DATA> SGE_INLINE
Vec4_Basic<T, DATA> max(const Vec4_Basic<T, DATA>& a, const Vec4_Basic<T, DATA>& b) {
	return Vec4_Basic<T, DATA>(	Math::max(a.x, b.x),
								Math::max(a.y, b.y),
								Math::max(a.z, b.z),
								Math::max(a.w, b.w));
}

template<class T, class DATA> SGE_INLINE
Vec4_Basic<T, DATA> clamp(const Vec4_Basic<T, DATA>& v, const Vec4_Basic<T, DATA>& a, const Vec4_Basic<T, DATA>& b) {
	return Vec4_Basic<T, DATA>(	Math::clamp(v.x, a.x, b.x),
								Math::clamp(v.y, a.y, b.y),
								Math::clamp(v.z, a.z, b.z),
								Math::clamp(v.w, a.w, b.w));
}

}} // namespace sge/Math
