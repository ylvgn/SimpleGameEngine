#pragma once

namespace sge {

template<typename T>
struct TVec2 {

	union {
		struct { T x, y; };
		T v[2];
	};

	inline TVec2()					: x(0), y(0)			{ }
	inline TVec2(T _x, T _y)		: x(_x), y(_y)			{ }
	inline TVec2(T* fv)				: x(fv[0]), y(fv[1])	{ }
	inline TVec2(const TVec2& fv)	: x(fv.x), y(fv.y)		{ }

			T& operator[](int i)		{ return v[i]; }
	const	T& operator[](int i) const	{ return v[i]; }
};

using vec2		= TVec2<float>;
using ivec2		= TVec2<int>;
using uivec2	= TVec2<unsigned int>;

}