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

	SGE_INLINE void set(const T& x_, const T& y_, const T& w_, const T& h_) {
		x = x_; y = y_; w = w_; h = h_;
	}

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

	SGE_INLINE void setByLTRB(const T& left, const T& top, const T& right, const T& bottom) {
		x = left;
		y = top;
		w = right - left;
		h = bottom - top;
	}

	SGE_INLINE Vec2 center() const {
		return Vec2(xMin() + xMax(), yMin() + yMax()) * T(0.5);
	}

	SGE_INLINE bool isInside(const Vec2& pt) const {
		return pt.x >= xMin() && pt.x <= xMax()
			&& pt.y >= yMin() && pt.y <= yMax();
	}

	Rect2<T> intersectRect(const Rect2& r) const {
		T left		= Math::max(xMin(), r.xMin());
		T top		= Math::max(yMin(), r.yMin());
		T right		= Math::min(xMax(), r.xMax());
		T bottom	= Math::min(yMax(), r.yMax());
		auto o = Rect2(left, top, right - left, bottom - top);
		if (!isInside(o.center())) {
			o.x = o.y = o.w = o.h = 0;
		}
		return o;
	}
};

using Rect2i = Rect2<int>;
using Rect2f = Rect2<float>;

}