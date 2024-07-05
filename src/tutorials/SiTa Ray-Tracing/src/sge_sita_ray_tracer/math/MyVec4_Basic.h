#pragma once

#include "MyTuple4.h"
#include "MyVec3.h"

namespace sge {

template<typename T, class DATA = MyTuple4<T> >
struct MyVec4_Basic : public DATA {
	using MyVec4 = MyVec4_Basic;
	using MyVec3 = MyVec3<T>;
	using MyVec2 = MyVec2<T>;

	using typename DATA::ElementType;

	using DATA::x;
	using DATA::y;
	using DATA::z;
	using DATA::w;
	using DATA::data;
	using DATA::kElementCount;

	constexpr MyVec4() = default;
	constexpr MyVec4(const T& x_, const T& y_, const T& z_, const T& w_) : DATA(x_,y_,z_,w_) {}
	constexpr MyVec4(const MyVec2& v, const T& z_, const T& w_) : DATA(v.x, v.y, z_, w_) {}
	constexpr MyVec4(const MyVec3& v, const T& w_) : DATA(v.x, v.y, v.z, w_) {}

	SGE_INLINE static MyVec4 s_zero()	{ return MyVec4(0,0,0,0); }
	SGE_INLINE static MyVec4 s_one()	{ return MyVec4(1,1,1,1); }

	SGE_INLINE MyVec2 xy()		const { return MyVec2(x, y); }
	SGE_INLINE MyVec2 xz()		const { return MyVec2(x, z); }
	SGE_INLINE MyVec2 yz()		const { return MyVec2(y, z); }
	SGE_INLINE MyVec3 xyz()		const { return MyVec3(x, y, z); }

	SGE_INLINE T dot(const MyVec4& r) const { return (x*r.x) + (y*r.y) + (z*r.z) + (w*r.w); }

	SGE_INLINE MyVec3 toVec3() const {
		if (!w) return MyVec3::s_zero();
		auto invW = T(1) / w;
		return MyVec3(x,y,z) * invW;
	}

	SGE_INLINE bool equals (const MyVec4& r, const T& epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(				 const T& epsilon = Math::epsilon<T>()) const;

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const	{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }

	SGE_INLINE MyVec4 operator+ (const MyVec4& r) const { return MyVec4(x+r.x, y+r.y, z+r.z, w+r.w); }
	SGE_INLINE MyVec4 operator- (const MyVec4& r) const { return MyVec4(x-r.x, y-r.y, z-r.z, w-r.w); }
	SGE_INLINE MyVec4 operator* (const MyVec4& r) const { return MyVec4(x*r.x, y*r.y, z*r.z, w*r.w); }
	SGE_INLINE MyVec4 operator/ (const MyVec4& r) const { return MyVec4(x/r.x, y/r.y, z/r.z, w/r.w); }

	SGE_INLINE MyVec4 operator+ (const T& s) const { return MyVec4(x+s, y+s, z+s, w+s); }
	SGE_INLINE MyVec4 operator- (const T& s) const { return MyVec4(x-s, y-s, z-s, w-s); }
	SGE_INLINE MyVec4 operator* (const T& s) const { return MyVec4(x*s, y*s, z*s, w*s); }
	SGE_INLINE MyVec4 operator/ (const T& s) const { return MyVec4(x/s, y/s, z/s, w/s); }

	SGE_INLINE void operator+= (const MyVec4& r) { x+=r.x; y+=r.y; z+=r.z; w+=r.w; }
	SGE_INLINE void operator-= (const MyVec4& r) { x-=r.x; y-=r.y; z-=r.z; w-=r.w; }
	SGE_INLINE void operator*= (const MyVec4& r) { x*=r.x; y*=r.y; z*=r.z; w*=r.w; }
	SGE_INLINE void operator/= (const MyVec4& r) { x/=r.x; y/=r.y; z/=r.z; w/=r.w; }

	SGE_INLINE void operator+= (const T& s) { x+=s; y+=s; z+=s; w+=s; }
	SGE_INLINE void operator-= (const T& s) { x-=s; y-=s; z-=s; w-=s; }
	SGE_INLINE void operator*= (const T& s) { x*=s; y*=s; z*=s; w*=s; }
	SGE_INLINE void operator/= (const T& s) { x/=s; y/=s; z/=s; w/=s; }

	SGE_INLINE MyVec4 operator-() const { return MyVec4(-x, -y, -z, -w); }

	SGE_INLINE bool operator== (const MyVec4& r) const	{ return x == r.x && y == r.y && z == r.z && w == r.w; }
	SGE_INLINE bool operator!= (const MyVec4& r) const	{ return !(this->operator==(r)); }

	SGE_INLINE operator MyVec3() const { return toVec3(); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, z, w);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const MyVec4& v) {
		o << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
		return o;
	}
};

using MyVec4_Basicf	= MyVec4_Basic<float>;
using MyVec4_Basicd	= MyVec4_Basic<double>;

SGE_FORMATTER_T( SGE_ARGS(typename T, class DATA), MyVec4_Basic< SGE_ARGS(T, DATA) >)


template<class T, class DATA> SGE_INLINE
bool MyVec4_Basic<T, DATA>::equals(const MyVec4_Basic<T, DATA>& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon)
		&& Math::equals(w, r.w, epsilon);
}

template<class T, class DATA> SGE_INLINE
bool MyVec4_Basic<T, DATA>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon)
		&& Math::equals0(w, epsilon);
}


namespace Math {

template<class T, class DATA> SGE_INLINE
void sincos(const MyVec4_Basic<T, DATA>& th, MyVec4_Basic<T, DATA>& outSin, MyVec4_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
	Math::sincos(th.z, outSin.z, outCos.z);
	Math::sincos(th.w, outSin.w, outCos.w);
}

template<class T, class DATA> SGE_INLINE
MyVec4_Basic<T, DATA> min(const MyVec4_Basic<T, DATA>& a, const MyVec4_Basic<T, DATA>& b) {
	return MyVec4_Basic<T, DATA>(Math::min(a.x, b.x),
								 Math::min(a.y, b.y),
								 Math::min(a.z, b.z),
								 Math::min(a.w, b.w));
}

template<class T, class DATA> SGE_INLINE
MyVec4_Basic<T, DATA> max(const MyVec4_Basic<T, DATA>& a, const MyVec4_Basic<T, DATA>& b) {
	return MyVec4_Basic<T, DATA>(Math::max(a.x, b.x),
								 Math::max(a.y, b.y),
								 Math::max(a.z, b.z),
								 Math::max(a.w, b.w));
}

} // namespace Math
} // namespace sge
