#pragma once

namespace sge {

template<typename T>
struct TVec2 {

	static const size_t kElementCount = 2;

	union {
		struct { T x, y; };
		T data[kElementCount];
	};

	inline TVec2() = default;
	inline TVec2(const T& x_, const T& y_)
		: x(x_), y(y_) {}

	inline void set(const T& x_, const T& y_) { x = x_; y = y_; }

	inline static TVec2 s_zero()	{ return TVec2(0,0); }
	inline static TVec2 s_one()		{ return TVec2(1,1); }

	inline			T& operator[](int i)		{ SGE_ASSERT(i < kElementCount); return data[i]; }
	inline const	T& operator[](int i) const	{ SGE_ASSERT(i < kElementCount); return data[i]; }

	inline bool equals (const TVec2& r, const T& epsilon = Math::epsilon<T>()) const;
	inline bool equals0(				const T& epsilon = Math::epsilon<T>()) const;

	inline TVec2 operator+ (const TVec2& r) const	{ return TVec2(x+r.x, y+r.y); }
	inline TVec2 operator- (const TVec2& r) const	{ return TVec2(x-r.x, y-r.y); }
	inline TVec2 operator* (const TVec2& r) const	{ return TVec2(x*r.x, y*r.y); }
	inline TVec2 operator/ (const TVec2& r) const	{ return TVec2(x/r.x, y/r.y); }

	inline TVec2 operator+ (const T& s) const		{ return TVec2(x+s, y+s); }
	inline TVec2 operator- (const T& s) const		{ return TVec2(x-s, y-s); }
	inline TVec2 operator* (const T& s) const		{ return TVec2(x*s, y*s); }
	inline TVec2 operator/ (const T& s) const		{ return TVec2(x/s, y/s); }

	inline void operator+= (const TVec2& r)			{ x+=r.x; y+=r.y; }
	inline void operator-= (const TVec2& r)			{ x-=r.x; y-=r.y; }
	inline void operator*= (const TVec2& r)			{ x*=r.x; y*=r.y; }
	inline void operator/= (const TVec2& r)			{ x/=r.x; y/=r.y; }

	inline void operator+= (const T& s)				{ x+=s; y+=s; }
	inline void operator-= (const T& s)				{ x-=s; y-=s; }
	inline void operator*= (const T& s)				{ x*=s; y*=s; }
	inline void operator/= (const T& s)				{ x/=s; y/=s; }

	inline TVec2 operator-() const					{ return TVec2(-x,-y); }

	inline bool operator== (const TVec2& r) const	{ return x == r.x && y == r.y; }
	inline bool operator!= (const TVec2& r) const	{ return !(this->operator==(r)); }

	inline			T* ptr()						{ return data; }
	inline const	T* ptr() const					{ return data; }

	inline void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {})", x, y);
	}

	inline friend std::ostream& operator<< (std::ostream& o, const TVec2& v) {
		o << "(" << v.x << ", " << v.y << ")";
		return o;
	}
};

template<class T> inline
bool TVec2<T>::equals(const TVec2<T>& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon);
}

template<class T> inline
bool TVec2<T>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon);
}

template<>
struct TVec2<int> {
	using ElementType = int;
	
	static const size_t kElementCount = 2;

	union {
		struct { ElementType x, y; };
		ElementType data[kElementCount];
	};

	inline TVec2<ElementType>() = default;
	inline TVec2(const ElementType& x_, const ElementType& y_)
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
	
	inline TVec2<ElementType>() = default;
	inline TVec2(const ElementType& x_, const ElementType& y_)
		: x(x_), y(y_) {}
};

using vec2f		= TVec2<float>;
using vec2d		= TVec2<double>;
using vec2i		= TVec2<int>;
using vec2ui	= TVec2<unsigned int>;

SGE_FORMATTER_T(class T, TVec2<T>)

}