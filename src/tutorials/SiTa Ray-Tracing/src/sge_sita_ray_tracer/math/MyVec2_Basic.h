#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/log/Log.h>
#include <sge_core/math/Mat4.h>
#include "MyTuple2.h"

namespace sge {

template<typename T, class DATA = MyTuple2<T> >
struct MyVec2_Basic : public DATA {
	using MyVec2 = MyVec2_Basic;

	using typename DATA::ElementType;

	using DATA::x;
	using DATA::y;
	using DATA::data;
	using DATA::kElementCount;

	constexpr MyVec2_Basic() = default;
	constexpr MyVec2_Basic(const T& x_, const T& y_) : DATA(x_,y_) {}

	SGE_INLINE static MyVec2 s_zero()	{ return MyVec2(0,0); }
	SGE_INLINE static MyVec2 s_one()	{ return MyVec2(1,1); }

	SGE_INLINE bool equals (const MyVec2& r, const T& epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(				 const T& epsilon = Math::epsilon<T>()) const;

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const	{ SGE_ASSERT(i >= 0 && i < kElementCount); return data[i]; }

	SGE_INLINE MyVec2 operator+ (const MyVec2& r) const { return MyVec2(x + r.x, y + r.y); }
	SGE_INLINE MyVec2 operator- (const MyVec2& r) const { return MyVec2(x - r.x, y - r.y); }
	SGE_INLINE MyVec2 operator* (const MyVec2& r) const { return MyVec2(x * r.x, y * r.y); }
	SGE_INLINE MyVec2 operator/ (const MyVec2& r) const { return MyVec2(x / r.x, y / r.y); }

	SGE_INLINE MyVec2 operator+ (const T& s) const { return MyVec2(x + s, y + s); }
	SGE_INLINE MyVec2 operator- (const T& s) const { return MyVec2(x - s, y - s); }
	SGE_INLINE MyVec2 operator* (const T& s) const { return MyVec2(x * s, y * s); }
	SGE_INLINE MyVec2 operator/ (const T& s) const { return MyVec2(x / s, y / s); }

	SGE_INLINE void operator+= (const MyVec2& r) { x += r.x; y += r.y; }
	SGE_INLINE void operator-= (const MyVec2& r) { x -= r.x; y -= r.y; }
	SGE_INLINE void operator*= (const MyVec2& r) { x *= r.x; y *= r.y; }
	SGE_INLINE void operator/= (const MyVec2& r) { x /= r.x; y /= r.y; }

	SGE_INLINE void operator+= (const T& s) { x += s; y += s; }
	SGE_INLINE void operator-= (const T& s) { x -= s; y -= s; }
	SGE_INLINE void operator*= (const T& s) { x *= s; y *= s; }
	SGE_INLINE void operator/= (const T& s) { x /= s; y /= s; }

	SGE_INLINE MyVec2 operator-() const { return MyVec2(-x,-y); }

	SGE_INLINE bool operator== (const MyVec2& r) const { return x == r.x && y == r.y; }
	SGE_INLINE bool operator!= (const MyVec2& r) const { return !(this->operator==(r)); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {})", x, y);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const MyVec2& v) {
		o << "(" << v.x << ", " << v.y << ")";
		return o;
	}
};

using MyVec2_Basicf = MyVec2_Basic<float>;
using MyVec2_Basicd = MyVec2_Basic<double>;

SGE_FORMATTER_T(SGE_ARGS(typename T, class DATA), MyVec2_Basic< SGE_ARGS(T, DATA) >)


template<typename T, class DATA> SGE_INLINE
bool MyVec2_Basic<T, DATA>::equals(const MyVec2_Basic<T, DATA>& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon) && Math::equals(y, r.y, epsilon);
}

template<typename T, class DATA> SGE_INLINE
bool MyVec2_Basic<T, DATA>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon) && Math::equals0(y, epsilon);
}


namespace Math {

template<class T, class DATA> SGE_INLINE
void sincos(const MyVec2_Basic<T, DATA>& th, MyVec2_Basic<T, DATA>& outSin, MyVec2_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
}

template<class T, class DATA> SGE_INLINE
MyVec2_Basic<T, DATA> min(const MyVec2_Basic<T, DATA>& a, const MyVec2_Basic<T, DATA>& b) {
	return MyVec2_Basic<T, DATA>(Math::min(a.x, b.x), Math::min(a.y, b.y));
}

template<class T, class DATA> SGE_INLINE
MyVec2_Basic<T, DATA> max(const MyVec2_Basic<T, DATA>& a, const MyVec2_Basic<T, DATA>& b) {
	return MyVec2_Basic<T, DATA>(Math::max(a.x, b.x), Math::max(a.y, b.y));
}

} // namespace Math
} // namespace sge