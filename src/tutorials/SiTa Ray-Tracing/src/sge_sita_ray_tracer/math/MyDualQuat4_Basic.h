#pragma once

#include "MyQuat4.h"

namespace sge {

template<typename T>
struct MyDualQuat4_Basic_Data {
	static constexpr size_t kElementCount = 8;

	using ElementType = T;
	using MyVec3	= MyVec3<T>;
	using MyQuat4	= MyQuat4<T>;

	union {
		struct { T x, y, z, w, dx, dy, dz, dw; };
		struct { MyQuat4 real, dual; };
		T data[kElementCount];
	};

	constexpr MyDualQuat4_Basic_Data(const MyQuat4& real_, const MyQuat4& dual_)
		: real(real_), dual(dual_) {}

	constexpr MyDualQuat4_Basic_Data(T x_, T y_, T z_, T w_, T dx_, T dy_, T dz_, T dw_)
		: x(x_), y(y_), z(z_), w(w_)
		, dx(dx_), dy(dy_), dz(dz_), dw(dw_) {}

	constexpr MyDualQuat4_Basic_Data(const MyQuat4& q, const MyVec3& t)
		: x(q.x)
		, y(q.y)
		, z(q.z)
		, w(q.w)
		, dx( T(0.5) * ( t.x*w + t.y*z - t.z*y ))
		, dy( T(0.5) * (-t.x*z + t.y*w + t.z*x ))
		, dz( T(0.5) * ( t.x*y - t.y*x + t.z*w ))
		, dw(-T(0.5) * ( t.x*x + t.y*y + t.z*z )) {}

	SGE_INLINE void set(const MyQuat4& real_, const MyQuat4& dual_) {
		real = real_;
		dual = dual_;
	}

	SGE_INLINE void set(T x_, T y_, T z_, T w_, T dx_, T dy_, T dz_, T dw_) {
		setReal(x_, y_, z_, w_);
		setDual(dx_, dy_, dz_, dw_);
	}

	SGE_INLINE void set(const MyQuat4& q, const MyVec3& t) {
		real.set(q.x, q.y, q.z, q.w);
		dual.set(T(0.5) * ( t.x*w + t.y*z - t.z*y ),
				 T(0.5) * (-t.x*z + t.y*w + t.z*x ),
				 T(0.5) * ( t.x*y - t.y*x + t.z*w ),
				-T(0.5) * ( t.x*x + t.y*y + t.z*z )
		);
	}

	SGE_INLINE void setAll(const T& v) { set(v,v,v,v, v,v,v,v); }

	SGE_INLINE void setReal(const T& x_, const T& y_, const T& z_, const T& w_) {
		x = x_;	y = y_; z = z_; w = w_;
	}

	SGE_INLINE void setDual(const T& dx_, const T& dy_, const T& dz_, const T& dw_) {
		dx = dx_; dy = dy_; dz = dz_; dw = dw_;
	}
};

template<typename T, class DATA = MyDualQuat4_Basic_Data<T> >
struct MyDualQuat4_Basic : public DATA {
	using MyDualQuat4 = MyDualQuat4_Basic;

	using typename DATA::ElementType;
	using typename DATA::MyVec3;
	using typename DATA::MyQuat4;

	using DATA::x;
	using DATA::y;
	using DATA::z;
	using DATA::w;
	using DATA::dx;
	using DATA::dy;
	using DATA::dz;
	using DATA::dw;
	using DATA::real;
	using DATA::dual;
	using DATA::data;
	using DATA::kElementCount;

	constexpr MyDualQuat4() = default;
	constexpr MyDualQuat4(const MyQuat4& real_, const MyQuat4& dual_) : DATA(real_, dual_) {}
	constexpr MyDualQuat4(T x_, T y_, T z_, T w_, T dx_, T dy_, T dz_, T dw_) : DATA(x_,y_,z_,w_,dx_,dy_,dz_,dw_) {}
	constexpr MyDualQuat4(const MyQuat4& q, const MyVec3& t) : DATA(q, t) {}

	SGE_INLINE static MyDualQuat4 s_identity()	{ return MyDualQuat4(0,0,0,1, 0,0,0,0); }
	SGE_INLINE static MyDualQuat4 s_zero()		{ return MyDualQuat4(0,0,0,0, 0,0,0,0); }

	SGE_INLINE T dot(const MyDualQuat4& r)	const { return real.dot(r.real); }

	SGE_INLINE T sqrMagnitude()				const { return dot(*this); }
	SGE_INLINE T magnitude()				const { return Math::sqrt(sqrMagnitude()); }

	SGE_INLINE MyDualQuat4 normalize() const;
	SGE_INLINE void normalized();

	SGE_INLINE MyDualQuat4 conjugate() const { return MyDualQuat4(real.conjugate(), dual.conjugate()); }

	SGE_INLINE MyQuat4	toRotation() const { return real; }
	SGE_INLINE MyVec3	toTranslation() const;
	SGE_INLINE void		toRotationTranslation(MyQuat4& outQuat, MyVec3& outTranslation) const;

	SGE_INLINE MyVec3 mulVector(const MyVec3& v) const;
	SGE_INLINE MyVec3 mulPoint (const MyVec3& v) const;

	SGE_INLINE bool equals (const MyDualQuat4& r,	T epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(						T epsilon = Math::epsilon<T>()) const;

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const	{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }

	SGE_INLINE MyDualQuat4 operator+ (const MyDualQuat4& r) const { return MyDualQuat4(real+r.real, dual+r.dual); }
	SGE_INLINE MyDualQuat4 operator- (const MyDualQuat4& r) const { return MyDualQuat4(real-r.real, dual-r.dual); }

	SGE_INLINE MyDualQuat4 operator+ (const T& s) const { return MyDualQuat4(real+s, dual+s); }
	SGE_INLINE MyDualQuat4 operator- (const T& s) const { return MyDualQuat4(real-s, dual-s); }
	SGE_INLINE MyDualQuat4 operator* (const T& s) const { return MyDualQuat4(real*s, dual*s); }
	SGE_INLINE MyDualQuat4 operator/ (const T& s) const { return MyDualQuat4(real/s, dual/s); }

	SGE_INLINE void operator+= (const MyDualQuat4& r) { real+=r.real, dual+=r.dual; }
	SGE_INLINE void operator-= (const MyDualQuat4& r) { real-=r.real, dual-=r.dual; }

	SGE_INLINE void operator+= (const T& s) { real+=s, dual+=s; }
	SGE_INLINE void operator-= (const T& s) { real-=s, dual-=s; }
	SGE_INLINE void operator*= (const T& s) { real*=s, dual*=s; }
	SGE_INLINE void operator/= (const T& s) { real/=s, dual/=s; }

	SGE_INLINE MyDualQuat4 operator* (const MyDualQuat4& r) const {
		MyDualQuat4 lhs = this->normalize();
		MyDualQuat4 rhs = r.normalize();
		return MyDualQuat4(lhs.real * rhs.real, (lhs.real * rhs.dual) + (lhs.dual * rhs.real));
	}

	SGE_INLINE bool operator== (const MyDualQuat4& r) const { return real == r.real && dual == r.dual; }
	SGE_INLINE bool operator!= (const MyDualQuat4& r) const { return !this->operator==(r); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({},{},{},{}, {},{},{},{})", x, y, z, w, dx, dy, dz, dw);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const MyDualQuat4& q) {
		o << "(" \
		<< "\n\t" << q.x << "," << q.y << "," << q.z << "," << q.w << ", " \
		<< "\n\t" << q.dx << "," << q.dy << "," << q.dz << "," << q.dw \
		<< "\n)";
		return o;
	}
};

using MyDualQuat4_Basicf = MyDualQuat4_Basic<float>;
using MyDualQuat4_Basicd = MyDualQuat4_Basic<double>;

SGE_FORMATTER_T( SGE_ARGS(typename T, class DATA), MyDualQuat4_Basic< SGE_ARGS(T, DATA) >)

template<typename T, class DATA> SGE_INLINE
bool MyDualQuat4_Basic<T, DATA>::equals(const MyDualQuat4_Basic<T, DATA>& r, T epsilon) const {
	return real.equals(r.real, epsilon) && dual.equals(r.dual, epsilon);
}

template<typename T, class DATA> SGE_INLINE
bool MyDualQuat4_Basic<T, DATA>::equals0(T epsilon) const {
	return real.equals0(epsilon) && dual.equals0(epsilon);
}

template<typename T, class DATA> SGE_INLINE
MyDualQuat4_Basic<T, DATA> MyDualQuat4_Basic<T, DATA>::normalize() const {
	auto norm = real.sqrMagnitude();
	if (Math::equals0(norm)) {
		return MyDualQuat4::s_zero();
	}
	auto m = Math::sqrt(norm);
	auto inv = T(1) / m;
	return *this * inv;
}

template<typename T, class DATA> SGE_INLINE
void MyDualQuat4_Basic<T, DATA>::normalized() {
	auto norm = real.sqrMagnitude();
	if (Math::equals0(norm))
		return;

	auto m = Math::sqrt(norm);
	auto inv = T(1) / m;
	*this *= inv;
}

template<typename T, class DATA> SGE_INLINE
typename MyDualQuat4_Basic<T, DATA>::MyVec3 MyDualQuat4_Basic<T, DATA>::toTranslation() const {
	MyQuat4 q = real.conjugate() * dual * T(2);
	return MyVec3(q.x, q.y, q.z);
}

template<typename T, class DATA> SGE_INLINE
void MyDualQuat4_Basic<T, DATA>::toRotationTranslation(MyQuat4& outRotation, MyDualQuat4_Basic<T, DATA>::MyVec3& outTranslation) const {
	constexpr const T kTwo = T(2);
	outRotation.set(x, y, z, w);
	outTranslation.x = kTwo*(-dw*x + dx*w - dy*z + dz*y);
	outTranslation.y = kTwo*(-dw*y + dx*z + dy*w - dz*x);
	outTranslation.z = kTwo*(-dw*z - dx*y + dy*x + dz*w);
}

template<typename T, class DATA> SGE_INLINE
typename MyDualQuat4_Basic<T, DATA>::MyVec3 MyDualQuat4_Basic<T, DATA>::mulVector(const MyVec3& v) const {
	return real * v;
}

template<typename T, class DATA> SGE_INLINE
typename MyDualQuat4_Basic<T, DATA>::MyVec3 MyDualQuat4_Basic<T, DATA>::mulPoint(const MyVec3& v) const {
	// assume this dual quaternion is normalized
	return real * v + toTranslation();
}

}