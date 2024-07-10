#pragma once

#include "MyVec4.h"

namespace sge {

template<typename T, class DATA = MyTuple4<T> >
struct MyQuat4_Basic : public DATA {
	using MyQuat4	= MyQuat4_Basic;
	using MyVec3	= MyVec3<T>;

	using typename DATA::ElementType;

	using DATA::x;
	using DATA::y;
	using DATA::z;
	using DATA::w;
	using DATA::data;
	using DATA::kElementCount;

	constexpr MyQuat4() = default;
	constexpr MyQuat4(const T& x_, const T& y_, const T& z_, const T& w_) : DATA(x_,y_,z_,w_) {}

	SGE_INLINE void set(const MyQuat4& r) { DATA::set(r.x, r.y, r.z, r.w); }
	SGE_INLINE void set(const T& x_, const T& y_, const T& z_, const T& w_) { DATA::set(x_, y_, z_, w_); } // compile error without this, why need this ???

	SGE_INLINE static MyQuat4 s_identity()	{ return MyQuat4(0,0,0,1); }
	SGE_INLINE static MyQuat4 s_zero()		{ return MyQuat4(0,0,0,0); }

	SGE_INLINE static MyQuat4 s_angleAxis(const T& rad, const MyVec3& axis);
	SGE_INLINE static MyQuat4 s_fromTo(const MyVec3& from, const MyVec3& to);
	SGE_INLINE static MyQuat4 s_lookRotation(const MyVec3& dir, const MyVec3& up);

	SGE_INLINE static MyQuat4 s_euler(const MyVec3& r);
	SGE_INLINE static MyQuat4 s_eulerX(T rad)		{ return s_angleAxis(rad, MyVec3::s_right()); }
	SGE_INLINE static MyQuat4 s_eulerY(T rad)		{ return s_angleAxis(rad, MyVec3::s_up()); }
	SGE_INLINE static MyQuat4 s_eulerZ(T rad)		{ return s_angleAxis(rad, MyVec3::s_forward()); }

	SGE_INLINE static MyQuat4 s_eulerDegX(T deg)	{ return s_eulerX(Math::radians(deg)); }
	SGE_INLINE static MyQuat4 s_eulerDegY(T deg)	{ return s_eulerY(Math::radians(deg)); }
	SGE_INLINE static MyQuat4 s_eulerDegZ(T deg)	{ return s_eulerZ(Math::radians(deg)); }

	SGE_INLINE void setEuler(const MyVec3& r)		{ *this = s_euler(r); }
	SGE_INLINE void setEulerX(T rad)				{ *this = s_eulerX(rad); }
	SGE_INLINE void setEulerY(T rad)				{ *this = s_eulerY(rad); }
	SGE_INLINE void setEulerZ(T rad)				{ *this = s_eulerZ(rad); }

	SGE_INLINE void setEulerDegX(T deg)				{ *this = s_eulerDegX(deg); }
	SGE_INLINE void setEulerDegY(T deg)				{ *this = s_eulerDegY(deg); }
	SGE_INLINE void setEulerDegZ(T deg)				{ *this = s_eulerDegZ(deg); }

	SGE_INLINE T eulerX() const;
	SGE_INLINE T eulerY() const;
	SGE_INLINE T eulerZ() const;
	SGE_INLINE MyVec3 euler() const	{ return MyVec3(eulerX(), eulerY(), eulerZ()); }

	SGE_INLINE T	   angle() const { return T(2) * Math::acos(w); } // radians
	SGE_INLINE MyVec3  axis(bool isNomalized = false) const;

	SGE_INLINE bool equals (const MyQuat4& r,	T epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(					T epsilon = Math::epsilon<T>()) const;

	SGE_INLINE T dot(const MyQuat4& r)	const { return (x*r.x) + (y*r.y) + (z*r.z) + (w*r.w); }

	SGE_INLINE T sqrMagnitude()			const { return dot(*this); }
	SGE_INLINE T magnitude()			const { return Math::sqrt(sqrMagnitude()); }

	SGE_INLINE void		normalized();
	SGE_INLINE MyQuat4	normalize()		const;

	SGE_INLINE MyQuat4 conjugate()		const { return MyQuat4(-x, -y, -z, w); }

	SGE_INLINE MyQuat4 inverse()		const;
	SGE_INLINE MyQuat4 unitInverse()	const { return conjugate(); }

	SGE_INLINE MyQuat4 lerp (const MyQuat4& to, const T& t) const { return *this + (to - *this) * t; }
	SGE_INLINE MyQuat4 nlerp(const MyQuat4& to, const T& t)	const { return lerp(to, t).normalize(); }
	SGE_INLINE MyQuat4 slerp(const MyQuat4& to, const T& t, bool shortestPath = true) const;

	SGE_INLINE bool sameOrientation(const MyQuat4& r) const;

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const  { SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }

	SGE_INLINE MyQuat4 operator+ (const MyQuat4& r) const { return MyQuat4(x+r.x, y+r.y, z+r.z, w+r.w); }
	SGE_INLINE MyQuat4 operator- (const MyQuat4& r) const { return MyQuat4(x-r.x, y-r.y, z-r.z, w-r.w); }

	SGE_INLINE MyQuat4 operator+ (const T& s) const { return MyQuat4(x+s, y+s, z+s, w+s); }
	SGE_INLINE MyQuat4 operator- (const T& s) const { return MyQuat4(x-s, y-s, z-s, w-s); }
	SGE_INLINE MyQuat4 operator* (const T& s) const { return MyQuat4(x*s, y*s, z*s, w*s); }
	SGE_INLINE MyQuat4 operator/ (const T& s) const { return MyQuat4(x/s, y/s, z/s, w/s); }

	SGE_INLINE void operator+= (const T& s)	{ x+=s; y+=s; z+=s; w+=s; }
	SGE_INLINE void operator-= (const T& s)	{ x-=s; y-=s; z-=s; w-=s; }
	SGE_INLINE void operator*= (const T& s)	{ x*=s; y*=s; z*=s; w*=s; }
	SGE_INLINE void operator/= (const T& s)	{ x/=s; y/=s; z/=s; w/=s; }

	SGE_INLINE void operator+= (const MyQuat4& r)	{ x+=r.x; y+=r.y; z+=r.z; w+=r.w; }
	SGE_INLINE void operator-= (const MyQuat4& r)	{ x-=r.x; y-=r.y; z-=r.z; w-=r.w; }

	SGE_INLINE MyQuat4	operator* (const MyQuat4& r) const;
	SGE_INLINE MyVec3	operator* (const MyVec3& v) const;

	SGE_INLINE MyQuat4	operator^ (const T& t) const;
	SGE_INLINE MyQuat4	operator- () const { return MyQuat4(-x, -y, -z, -w); }

	SGE_INLINE bool operator== (const MyQuat4& r) const { return x == r.x && y == r.y && z == r.z && w == r.w; }
	SGE_INLINE bool operator!= (const MyQuat4& r) const { return !this->operator==(r); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, z, w);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const MyQuat4& q) {
		o << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
		return o;
	}
};

using MyQuat4_Basicf = MyQuat4_Basic<float>;
using MyQuat4_Basicd = MyQuat4_Basic<double>;

SGE_FORMATTER_T( SGE_ARGS(typename T, class DATA), MyQuat4_Basic< SGE_ARGS(T, DATA) >)

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::s_angleAxis(const T& rad, const MyVec3& axis) {
	// assume this axis is normalized
	T s, c;
	Math::sincos(rad * T(0.5), s, c);
	return MyQuat4(axis.x*s, axis.y*s, axis.z*s, c);
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::s_euler(const MyVec3& r) {
	MyVec3 s, c;
	Math::sincos(r * T(0.5), s, c);
	return MyQuat4(s.x * c.y * c.z - c.x * s.y * s.z,
				   c.x * s.y * c.z + s.x * c.y * s.z,
				   c.x * c.y * s.z - s.x * s.y * c.z,
				   c.x * c.y * c.z + s.x * s.y * s.z);
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::s_fromTo(const MyVec3& from, const MyVec3& to) {
	MyVec3 f = from.normalize();
	MyVec3 t = to.normalize();

	if (Math::equals(f, t)) {
		return MyQuat4::s_identity();
	}
	else if (Math::equals(f, -t)) {
		MyVec3 ortho = MyVec3::s_right();

		if (Math::abs(f.y) < Math::abs(f.x)) {
			ortho = MyVec3::s_up();
		}

		if (Math::abs(f.z) < Math::abs(f.y) && Math::abs(f.z) < Math::abs(f.x)) {
			ortho = MyVec3::s_forward();
		}

		MyVec3 axis = f.cross(ortho).normalize();
		return MyQuat4(axis.x, axis.y, axis.z, 0);
	}

	MyVec3 half = (f+t).normalize();
	MyVec3 axis = f.cross(half);
	return MyQuat4(axis.x, axis.y, axis.z, f.dot(half));
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::s_lookRotation(const MyVec3& dir, const MyVec3& up) {
	MyVec3 f			= dir.normalize();
	MyVec3 desiredUp	= up.normalize();
	MyVec3 r			= desiredUp.cross(f);
	desiredUp			= f.cross(r);

	MyQuat4 f2d			= MyQuat4::s_fromTo(MyVec3::s_forward(), f);
	MyVec3 objectUp		= f2d * MyVec3::s_up();
	MyQuat4 u2u			= MyQuat4::s_fromTo(objectUp, desiredUp);

	MyQuat4 result(f2d * u2u);
	return result.normalize();
}

template<typename T, class DATA> SGE_INLINE
bool MyQuat4_Basic<T, DATA>::equals(const MyQuat4_Basic<T, DATA>& r, T epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon)
		&& Math::equals(w, r.w, epsilon);
}

template<typename T, class DATA> SGE_INLINE
bool MyQuat4_Basic<T, DATA>::equals0(T epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon)
		&& Math::equals0(w, epsilon);
}

template<typename T, class DATA> SGE_INLINE
T MyQuat4_Basic<T, DATA>::eulerZ() const {
	return Math::atan2(T(2) * (x * y + w * z), w * w + x * x - y * y - z * z);
}

template<typename T, class DATA> SGE_INLINE
T MyQuat4_Basic<T, DATA>::eulerY() const {
	return Math::asin(Math::clamp(T(-2) * (x * z - w * y), T(-1), T(1)));
}

template<typename T, class DATA> SGE_INLINE
T MyQuat4_Basic<T, DATA>::eulerX() const {
	return Math::atan2(T(2) * (y * z + w * x), w * w - x * x - y * y + z * z);
}

template<typename T, class DATA> SGE_INLINE
typename MyQuat4_Basic<T, DATA>::MyVec3 MyQuat4_Basic<T, DATA>::axis(bool isNomalized) const {
	T sqCos = T(1) - w * w;
	if (sqCos <= T(0)) {
		return MyVec3::s_forward();
	}
	auto a = MyVec3(x, y, z);
	if (isNomalized) {
		return a.normalize();
	}
	T b = T(1) / Math::sqrt(sqCos);
	return a * b;
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::operator* (const MyQuat4& r) const {
	return MyQuat4(	 r.x*w + r.y*z - r.z*y + r.w*x,
					-r.x*z + r.y*w + r.z*x + r.w*y,
					 r.x*y - r.y*x + r.z*w + r.w*z,
					-r.x*x - r.y*y - r.z*z + r.w*w);
}

template<typename T, class DATA> SGE_INLINE
typename MyQuat4_Basic<T, DATA>::MyVec3 MyQuat4_Basic<T, DATA>::operator* (const MyVec3& v) const {
	MyVec3 qv(x, y, z);
	auto uv = qv.cross(v);
	auto uuv = qv.cross(uv);
	return v + (uv * w + uuv) * 2;
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::normalize() const {
	auto norm = sqrMagnitude();
	if (Math::equals0(norm)) {
		return MyQuat4::s_zero();
	}
	auto m = Math::sqrt(norm);
	auto inv = T(1) / m;
	return *this * inv;
}

template<typename T, class DATA> SGE_INLINE
void MyQuat4_Basic<T, DATA>::normalized() {
	auto norm = sqrMagnitude();
	if (Math::equals0(norm)) {
		return;
	}
	auto m = Math::sqrt(norm);
	auto inv = T(1) / m;
	*this *= inv;
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::inverse() const {
	auto m = sqrMagnitude();
	if (Math::equals0(m)) {
		return MyQuat4::s_zero();
	}
	auto inv = T(1) / m;
	return MyQuat4(-x * inv, -y * inv, -z * inv, w * inv);
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::operator^ (const T& t) const {
	auto theta = angle();
	MyVec3 a = MyVec3(x,y,z).normalize();

	T s, c;
	Math::sincos(t * (theta * T(0.5)), s, c);
	return MyQuat4(a.x * s, a.y * s, a.z * s, c);
}

template<typename T, class DATA> SGE_INLINE
MyQuat4_Basic<T, DATA> MyQuat4_Basic<T, DATA>::slerp(const MyQuat4& to, const T& t, bool shortestPath /*= true*/) const {
	MyQuat4 b = to;
	auto cosTheta = dot(to);

	if (cosTheta < T(0) && shortestPath) {
		b = -to;
		cosTheta = -cosTheta;
	}

	if (cosTheta > T(1) - Math::epsilon<T>()) {
		return nlerp(b, t);
	}

	T sinTheta = Math::sqrt(1 - cosTheta * cosTheta);
	T thetaInRad = Math::atan2(sinTheta, cosTheta);
	T invSin = T(1) / sinTheta;
	T coeffA = Math::sin((T(1) - t) * thetaInRad) * invSin;
	T coeffB = Math::sin(t * thetaInRad) * invSin;
	return (*this * coeffA) + (b * coeffB);
}

template<typename T, class DATA> SGE_INLINE
bool MyQuat4_Basic<T, DATA>::sameOrientation(const MyQuat4_Basic& r) const {
	return equals(r) 
		|| (Math::equals0(x + r.x)
		 && Math::equals0(y + r.y)
		 && Math::equals0(z + r.z)
		 && Math::equals0(w + r.w));
}

}