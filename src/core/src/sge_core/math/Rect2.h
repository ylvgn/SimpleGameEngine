#pragma once

#include "Vec2.h"

namespace sge {

template<class T>
struct Rect2 {
	using ElementType = T;
	using This		  = typename Rect2<T>;
	using Vec2		  = Vec2<T>;

	static const size_t kElementCount = 4;

	union {
		struct { T x, y, w, h; };
		struct { Vec2 pos, size; };
		T data[kElementCount];
	};

	SGE_INLINE constexpr explicit Rect2() = default;
	SGE_INLINE constexpr explicit Rect2(T v)									: x(v), y(v), w(v), h(v) {}
	SGE_INLINE constexpr		  Rect2(T x_, T y_, T w_, T h_)					: x(x_), y(y_), w(w_), h(h_) {}
	SGE_INLINE constexpr		  Rect2(const Vec2& pos_, const Vec2& size_)	: pos(pos_), size(size_) {}

	SGE_INLINE constexpr void set(const T& x_, const T& y_, const T& w_, const T& h_) {
		x = x_; y = y_; w = w_; h = h_;
	}

	SGE_INLINE constexpr void set(const Vec2& pos_, const Vec2& size_) {
		pos = pos_; size = size_;
	}

	SGE_INLINE constexpr T xMin() const { return x; }
	SGE_INLINE constexpr T yMin() const { return y; }

	SGE_INLINE constexpr T xMax() const { return x + w; }
	SGE_INLINE constexpr T yMax() const { return y + h; }

	SGE_INLINE constexpr bool operator == (const Rect2& r) const { return x == r.x && y == r.y && w == r.w && h == r.h; }
	SGE_INLINE constexpr bool operator != (const Rect2& r) const { return !(this->operator==(r)); }

	SGE_NODISCARD SGE_INLINE constexpr This offset(const Vec2& v) const { return axRect2(pos + v, size); }

	SGE_NODISCARD SGE_INLINE constexpr This operator + (const Vec2& v) const { return offset(v); }
	SGE_NODISCARD SGE_INLINE constexpr This operator - (const Vec2& v) const { return offset(-v); }

	SGE_NODISCARD SGE_INLINE constexpr Vec2 center() const {
		return Vec2(xMin() + xMax(), yMin() + yMax()) * T(0.5);
	}

	SGE_INLINE constexpr bool isPtInRect(const Vec2& pt) const {
		return pt.x >= xMin() && pt.x <= xMax()
			&& pt.y >= yMin() && pt.y <= yMax();
	}

	SGE_NODISCARD Rect2<T> intersectRect(const Rect2& r) const {
		T left		= Math::max(xMin(), r.xMin());
		T top		= Math::max(yMin(), r.yMin());
		T right		= Math::min(xMax(), r.xMax());
		T bottom	= Math::min(yMax(), r.yMax());
		auto o = Rect2(left, top, right - left, bottom - top);
		if (!isPtInRect(o.center())) {
			o.x = o.y = o.w = o.h = 0;
		}
		return o;
	}

	SGE_NODISCARD Rect2<T> unionRect(const Rect2& r) const {
		T left		= Math::min(xMin(), r.xMin());
		T top		= Math::min(yMin(), r.yMin());
		T right		= Math::max(xMax(), r.xMax());
		T bottom	= Math::max(yMax(), r.yMax());
		return Rect2(left, top, right - left, bottom - top);
	}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, w, h);
	}

#if SGE_OS_WINDOWS
	explicit constexpr Rect2(const RECT& r) { set(r); }

	SGE_INLINE constexpr void set(const RECT& src) {
		set(T(src.left),
			T(src.top),
			T(src.right - src.left),
			T(src.bottom - src.top)
		);
	}

	SGE_INLINE constexpr RECT toRECT() const {
		::RECT o;
		o.left = static_cast<LONG>(xMin());	o.right = static_cast<LONG>(xMax());
		o.top  = static_cast<LONG>(yMin());	o.bottom = static_cast<LONG>(yMax());
		return o;
	}
#endif // SGE_OS_WINDOWS
};

using Rect2i = Rect2<int>;
using Rect2f = Rect2<float>;

SGE_FORMATTER_T(class T, Rect2<T>)

}