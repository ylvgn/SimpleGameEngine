#pragma once

#include "Vec2.h"

namespace sge {

template<class T>
struct Rect2 {
public:
	using ElementType = T;
	static const size_t kElementCount = 4;

	using Vec2 = Vec2<T>;

	union {
		struct { T x, y, w, h; };
		struct { Vec2 pos, size; };
		T data[kElementCount];
	};

	SGE_INLINE Rect2() = default;
	constexpr SGE_INLINE Rect2(const T& x_, const T& y_, const T& w_, const T& h_) : x(x_), y(y_), w(w_), h(h_) {}
	SGE_INLINE Rect2(const Vec2& pos_, const Vec2& size_) : pos(pos_), size(size_) {}

	SGE_INLINE void set(const Vec2& pos_, const Vec2& size_) {
		pos = pos_;
		size = size_;
	}

	SGE_INLINE T xMin() const { return x; }
	SGE_INLINE T yMin() const { return y; }

	SGE_INLINE T xMax() const { return x + w; }
	SGE_INLINE T yMax() const { return y + h; }

	SGE_INLINE bool operator== (const Rect2& r) const { return x == r.x && y == r.y && w == r.w && h == r.h; }
	SGE_INLINE bool operator!= (const Rect2& r) const { return !(this->operator==(r)); }
};

using Rect2i = Rect2<int>;
using Rect2f = Rect2<float>;

}