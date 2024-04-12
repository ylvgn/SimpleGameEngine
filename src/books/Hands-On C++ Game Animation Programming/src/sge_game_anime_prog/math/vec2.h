#pragma once

namespace sge {

template<typename T>
struct TVec2 {

	static const size_t kElementCount = 2;

	union {
		struct { T x, y; };
		T data[kElementCount];
	};

	SGE_INLINE TVec2() = default;
	SGE_INLINE TVec2(const T& x_, const T& y_) : x(x_), y(y_) {}

	SGE_INLINE void set(const T& x_, const T& y_) { x = x_; y = y_; }

	SGE_INLINE static TVec2 s_zero()	{ return TVec2(0, 0); }
	SGE_INLINE static TVec2 s_one()		{ return TVec2(1, 1); }

	SGE_INLINE			T& operator[](int i)		{ SGE_ASSERT(i < kElementCount); return data[i]; }
	SGE_INLINE const	T& operator[](int i) const	{ SGE_ASSERT(i < kElementCount); return data[i]; }

	SGE_INLINE bool equals(const TVec2& r, const T& epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(const T& epsilon = Math::epsilon<T>()) const;

	SGE_INLINE TVec2 operator+ (const TVec2& r) const { return TVec2(x + r.x, y + r.y); }
	SGE_INLINE TVec2 operator- (const TVec2& r) const { return TVec2(x - r.x, y - r.y); }
	SGE_INLINE TVec2 operator* (const TVec2& r) const { return TVec2(x * r.x, y * r.y); }
	SGE_INLINE TVec2 operator/ (const TVec2& r) const { return TVec2(x / r.x, y / r.y); }

	SGE_INLINE TVec2 operator+ (const T& s) const { return TVec2(x + s, y + s); }
	SGE_INLINE TVec2 operator- (const T& s) const { return TVec2(x - s, y - s); }
	SGE_INLINE TVec2 operator* (const T& s) const { return TVec2(x * s, y * s); }
	SGE_INLINE TVec2 operator/ (const T& s) const { return TVec2(x / s, y / s); }

	SGE_INLINE void operator+= (const TVec2& r) { x += r.x; y += r.y; }
	SGE_INLINE void operator-= (const TVec2& r) { x -= r.x; y -= r.y; }
	SGE_INLINE void operator*= (const TVec2& r) { x *= r.x; y *= r.y; }
	SGE_INLINE void operator/= (const TVec2& r) { x /= r.x; y /= r.y; }

	SGE_INLINE void operator+= (const T& s) { x += s; y += s; }
	SGE_INLINE void operator-= (const T& s) { x -= s; y -= s; }
	SGE_INLINE void operator*= (const T& s) { x *= s; y *= s; }
	SGE_INLINE void operator/= (const T& s) { x /= s; y /= s; }

	SGE_INLINE TVec2 operator-() const { return TVec2(-x, -y); }

	SGE_INLINE bool operator== (const TVec2& r) const { return x == r.x && y == r.y; }
	SGE_INLINE bool operator!= (const TVec2& r) const { return !(this->operator==(r)); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {})", x, y);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const TVec2& v) {
		o << "(" << v.x << ", " << v.y << ")";
		return o;
	}
};

template<>
struct TVec2<int> {
	using ElementType = int;

	static const size_t kElementCount = 2;

	union {
		struct { ElementType x, y; };
		ElementType data[kElementCount];
	};

	SGE_INLINE TVec2<ElementType>() = default;
	SGE_INLINE TVec2(const ElementType& x_, const ElementType& y_)
		: x(x_), y(y_) {}
};

template<>
struct TVec2<unsigned int> {
	using ElementType = unsigned int;

	static const size_t kElementCount = 2;

	union {
		struct { ElementType x, y; };
		ElementType data[kElementCount];
	};

	SGE_INLINE TVec2<ElementType>() = default;
	SGE_INLINE TVec2(const ElementType& x_, const ElementType& y_)
		: x(x_), y(y_) {}
};

using vec2f		= TVec2<float>;
using vec2d		= TVec2<double>;
using vec2i		= TVec2<int>;
using vec2ui	= TVec2<unsigned int>;

SGE_FORMATTER_T(typename T, TVec2<T>)

#if 0
#pragma mark ------------------- instance functions -------------------
#endif
template<typename T> SGE_INLINE
bool TVec2<T>::equals(const TVec2<T>& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon);
}

template<typename T> SGE_INLINE
bool TVec2<T>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon);
}

}