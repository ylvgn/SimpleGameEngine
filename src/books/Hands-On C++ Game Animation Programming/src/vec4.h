#pragma once

namespace sge {

template<typename T>
struct TVec4 {
	union {
		struct {
			T x, y, z, w;
		};
		T v[4];
	};

	inline TVec4() : x(0), y(0), z(0), w(0) { }
	inline TVec4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) { }
	inline TVec4(T* fv) : x(fv[0]), y(fv[1]), z(fv[2]), w(fv[3]) { }
	inline TVec4(const TVec4& fv) : x(fv.x), y(fv.y), z(fv.z), w(fv.w) { }
};

using vec4 = TVec4<float>;
using ivec4 = TVec4<int>;
using uivec4 = TVec4<unsigned int>;

}
