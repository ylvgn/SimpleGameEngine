#pragma once

#include "MyTuple3.h"
#include "MyVec2.h"

namespace sge {

template<typename T, class DATA = MyTuple3<T> >
struct MyVec3_Basic : public DATA {
	using MyVec3 = MyVec3_Basic;
	using MyVec2 = MyVec2<T>;

	using typename DATA::ElementType;

	using DATA::x;
	using DATA::y;
	using DATA::z;
	using DATA::data;
	using DATA::kElementCount;

	constexpr MyVec3() = default;
	constexpr MyVec3(const T& x_, const T& y_, const T& z_) : DATA(x_,y_,z_) {}
	constexpr MyVec3(const sge::Vec3<T>& r) : DATA(r) {}

	SGE_INLINE static MyVec3 s_zero()		{ return MyVec3(0,  0,  0); }
	SGE_INLINE static MyVec3 s_one()		{ return MyVec3(1,  1,  1); }
	SGE_INLINE static MyVec3 s_forward()	{ return MyVec3(0,  0,  1); }
	SGE_INLINE static MyVec3 s_back()		{ return MyVec3(0,  0, -1); }
	SGE_INLINE static MyVec3 s_up()			{ return MyVec3(0,  1,  0); }
	SGE_INLINE static MyVec3 s_down()		{ return MyVec3(0, -1,  0); }
	SGE_INLINE static MyVec3 s_right()		{ return MyVec3(1,  0,  0); }
	SGE_INLINE static MyVec3 s_left()		{ return MyVec3(-1, 0,  0); }

	template<typename R, typename R_DATA>
	MyVec3 operator=(const MyVec3_Basic<R, R_DATA>& v) {
		return MyVec3(static_cast<T>(v.x), static_cast<T>(v.y), static_cast<T>(v.z));
	}

	SGE_INLINE MyVec2 xy() const { return MyVec2(x,y); }
	SGE_INLINE MyVec2 xz() const { return MyVec2(x,z); }
	SGE_INLINE MyVec2 yz() const { return MyVec2(y,z); }

	SGE_INLINE T		dot(const MyVec3& r)	const { return (x*r.x) + (y*r.y) + (z*r.z); }
	SGE_INLINE MyVec3	cross(const MyVec3& r)	const { return MyVec3(y*r.z - z*r.y, z*r.x - x*r.z, x*r.y - y*r.x); }

	SGE_INLINE MyVec3	normalize()				const { T m = magnitude(); return Math::equals0(m) ? s_zero() : (*this / m); }

	SGE_INLINE T		sqrMagnitude()			const { return dot(*this); }
	SGE_INLINE T		magnitude()				const { T m = sqrMagnitude(); return Math::equals0(m) ? T(0) : Math::sqrt(m); }

	SGE_INLINE T		radians(const MyVec3& r) const;
	SGE_INLINE T		degrees(const MyVec3& r) const { return Math::degrees(radians(r)); }

	SGE_INLINE MyVec3	project(const MyVec3& r) const;
	SGE_INLINE MyVec3	reject(const MyVec3& r)  const;
	SGE_INLINE MyVec3	reflect(const MyVec3& r) const;

	SGE_INLINE MyVec3	lerp (const MyVec3& to, const T& t) const;
	SGE_INLINE MyVec3	slerp(const MyVec3& to_, const T& t) const;
	SGE_INLINE MyVec3	nlerp(const MyVec3& to, const T& t) const { return lerp(to, t).normalize(); }

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const	{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }

	SGE_INLINE MyVec3 operator+ (const MyVec3& r) const { return MyVec3(x+r.x, y+r.y, z+r.z); }
	SGE_INLINE MyVec3 operator- (const MyVec3& r) const { return MyVec3(x-r.x, y-r.y, z-r.z); }
	SGE_INLINE MyVec3 operator* (const MyVec3& r) const { return MyVec3(x*r.x, y*r.y, z*r.z); }
	SGE_INLINE MyVec3 operator/ (const MyVec3& r) const { return MyVec3(x/r.x, y/r.y, z/r.z); }

	SGE_INLINE MyVec3 operator+ (const T& s) const { return MyVec3(x+s, y+s, z+s); }
	SGE_INLINE MyVec3 operator- (const T& s) const { return MyVec3(x-s, y-s, z-s); }
	SGE_INLINE MyVec3 operator* (const T& s) const { return MyVec3(x*s, y*s, z*s); }
	SGE_INLINE MyVec3 operator/ (const T& s) const { return MyVec3(x/s, y/s, z/s); }

	SGE_INLINE void operator+= (const MyVec3& r) { x+=r.x; y+=r.y; z+=r.z; }
	SGE_INLINE void operator-= (const MyVec3& r) { x-=r.x; y-=r.y; z-=r.z; }
	SGE_INLINE void operator*= (const MyVec3& r) { x*=r.x; y*=r.y; z*=r.z; }
	SGE_INLINE void operator/= (const MyVec3& r) { x/=r.x; y/=r.y; z/=r.z; }

	SGE_INLINE void operator+= (const T& s) { x+=s; y+=s; z+=s; }
	SGE_INLINE void operator-= (const T& s) { x-=s; y-=s; z-=s; }
	SGE_INLINE void operator*= (const T& s) { x*=s; y*=s; z*=s; }
	SGE_INLINE void operator/= (const T& s) { x/=s; y/=s; z/=s; }

	SGE_INLINE MyVec3 operator-() const { return MyVec3(-x,-y,-z); }

	SGE_INLINE bool operator== (const MyVec3& r) const { return x == r.x && y == r.y && z == r.z; }
	SGE_INLINE bool operator!= (const MyVec3& r) const { return !(this->operator==(r)); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {})", x, y, z);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const MyVec3& v) {
		o << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return o;
	}
};

using MyVec3_Basicf	= MyVec3_Basic<float>;
using MyVec3_Basicd	= MyVec3_Basic<double>;

SGE_FORMATTER_T( SGE_ARGS(typename T, class DATA), MyVec3_Basic< SGE_ARGS(T, DATA) >)


template<typename T, class DATA> SGE_INLINE
T MyVec3_Basic<T, DATA>::radians(const MyVec3_Basic<T, DATA>& r) const {
	T sqMagL = sqrMagnitude();
	T sqMagR = r.sqrMagnitude();
	if (Math::equals0(sqMagL) || Math::equals0(sqMagR)) {
		return T(0);
	}
	T cosTheta = dot(r) / Math::sqrt(sqMagL * sqMagR);
	return Math::acos(cosTheta);
}

template<typename T, class DATA> SGE_INLINE
MyVec3_Basic<T, DATA> MyVec3_Basic<T, DATA>::project(const MyVec3_Basic<T, DATA>& r) const {
	T sqMagR = r.sqrMagnitude();
	if (Math::equals0(sqMagR)) {
		SGE_LOG_ERROR("project zero vector");
		return MyVec3::s_zero();
	}
	T factor = dot(r) / sqMagR;
	return r * factor;
}

template<typename T, class DATA> SGE_INLINE
MyVec3_Basic<T, DATA> MyVec3_Basic<T, DATA>::reject(const MyVec3_Basic<T, DATA>& r) const {
	return *this - project(r);
}

template<typename T, class DATA> SGE_INLINE
MyVec3_Basic<T, DATA> MyVec3_Basic<T, DATA>::reflect(const MyVec3_Basic<T, DATA>& r) const {
	auto proj = project(r);
	if (Math::equals0(proj)) {
		SGE_LOG_ERROR("reflect zero vector");
		return MyVec3::s_zero();
	}

	auto proj2 = proj + proj;
	return *this - proj2;
}

template<typename T, class DATA> SGE_INLINE
MyVec3_Basic<T, DATA> MyVec3_Basic<T, DATA>::lerp(const MyVec3_Basic& to, const T& t) const {
	return MyVec3_Basic(Math::lerp(x, to.x, t),
						Math::lerp(y, to.y, t),
						Math::lerp(z, to.z, t));
}

template<typename T, class DATA> SGE_INLINE
MyVec3_Basic<T, DATA> MyVec3_Basic<T, DATA>::slerp(const MyVec3_Basic<T, DATA>& to_, const T& t) const {
	if (Math::equals0(Math::abs(t - T(0.01)))) {
		return lerp(to_, t);
	}

	MyVec3 from = this->normalize();
	MyVec3 to	= to_.normalize();

	T theta		= from.radians(to);
	T sinTheta	= Math::sin(theta);

	T a = Math::sin((T(1) - t) * theta) / sinTheta;
	T b = Math::sin(t * theta) / sinTheta;
	return (from*a) + (to * b);
}

#if 0
#pragma mark ========= Math ============
#endif
namespace Math {

template<class T, class DATA> SGE_INLINE
bool equals(const MyVec3_Basic<T, DATA>& a, const MyVec3_Basic<T, DATA>& b, const T& epsilon = Math::epsilon<T>()) {
	return Math::equals(a.x, b.x, epsilon)
		&& Math::equals(a.y, b.y, epsilon)
		&& Math::equals(a.z, b.z, epsilon);
}

template<class T, class DATA> SGE_INLINE
bool equals0(const MyVec3_Basic<T, DATA>& v, const T& epsilon = Math::epsilon<T>()) {
	return Math::equals0(v.x, epsilon)
		&& Math::equals0(v.y, epsilon)
		&& Math::equals0(v.z, epsilon);
}

template<class T, class DATA> SGE_INLINE
void sincos(const MyVec3_Basic<T, DATA>& th, MyVec3_Basic<T, DATA>& outSin, MyVec3_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
	Math::sincos(th.z, outSin.z, outCos.z);
}

template<class T, class DATA> SGE_INLINE
MyVec3_Basic<T, DATA> min(const MyVec3_Basic<T, DATA>& a, const MyVec3_Basic<T, DATA>& b) {
	return MyVec3_Basic<T, DATA>(Math::min(a.x, b.x),
								 Math::min(a.y, b.y),
								 Math::min(a.z, b.z));
}

template<class T, class DATA> SGE_INLINE
MyVec3_Basic<T, DATA> max(const MyVec3_Basic<T, DATA>& a, const MyVec3_Basic<T, DATA>& b) {
	return MyVec3_Basic<T, DATA>(Math::max(a.x, b.x),
								 Math::max(a.y, b.y),
								 Math::max(a.z, b.z));
}

} // namespace Math
} // namespace sge
