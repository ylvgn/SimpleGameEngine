#pragma once

#if SGE_OS_WINDOWS

#include "PW5_GDI__Color.h"
#include "PW5_GDI__Text.h"

namespace sge {

#define PW5_HatchStyle_ENUM_LIST(E)	\
	E(Horizontal,	= HS_HORIZONTAL	) \
	E(Vertical,		= HS_VERTICAL	) \
	E(Fdiagonal,	= HS_FDIAGONAL	) \
	E(Bdiagonal,	= HS_BDIAGONAL	) \
	E(Cross,		= HS_CROSS		) \
	E(DiagCross,	= HS_DIAGCROSS	) \
//----
SGE_ENUM_CLASS(PW5_HatchStyle, u8)

#define PW5_PenStyle_ENUM_LIST(E) \
	E(None,			= PS_NULL		) \
	E(Solid,		= PS_SOLID		) \
	E(Dash,			= PS_DASH		) \
	E(Dot,			= PS_DOT		) \
	E(DashDot,		= PS_DASHDOT	) \
	E(DashDotDot,	= PS_DASHDOTDOT	) \
//----
SGE_ENUM_CLASS(PW5_PenStyle, u8)

namespace GDI {

	using HSFlag	= PW5_HatchStyle;
	using PenStyle	= PW5_PenStyle;

	inline int drawText(const ::HDC& hdc, int left, int top, int right, int bottom, StrView str, UINT flags) {
		if (str.empty()) return 0;
		::RECT ltrb { left, top, right, bottom };
		auto s = UtfUtil::toStringW(str);
		return ::DrawText(hdc, s.c_str(), static_cast<int>(str.size()), &ltrb, flags);
	}
	inline int drawText( const ::HDC& hdc, int left, int top, int right, int bottom, StrView str,
						 DTFlag flags = DTFlag::SingleLine | DTFlag::Center | DTFlag::VCenter) {
		return GDI::drawText(hdc, left, top, right, bottom, str, static_cast<UINT>(flags));
	}
	inline int drawText(const ::HDC& hdc, const ::RECT& ltrb, StrView str, DTFlag flags) {
		return drawText(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, str, flags);
	}
	inline int drawText(const ::HDC& hdc, const Rect2f& xywh, StrView str, DTFlag flags) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return drawText(hdc, ltrb, str, flags);
	}

	inline bool rectangle(const ::HDC& hdc, int left, int top, int right, int bottom) {
		return ::Rectangle(hdc, left, top, right, bottom);
	}
	inline bool rectangle(const ::HDC& hdc, const ::RECT& ltrb) {
		return ::Rectangle(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom);
	}
	inline bool rectangle(const ::HDC& hdc, const Rect2f& xywh) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return rectangle(hdc, ltrb);
	}

	inline bool ellipse(const ::HDC& hdc, int left, int top, int right, int bottom) {
		return ::Ellipse(hdc, left, top, right, bottom);
	}
	inline bool ellipse(const ::HDC& hdc, const ::RECT& ltrb) {
		return ::Ellipse(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom);
	}
	inline bool ellipse(const ::HDC& hdc, const Rect2f& xywh) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return ellipse(hdc, ltrb);
	}

	inline bool roundRect(const ::HDC& hdc, int left, int top, int right, int bottom,
						  int xEllipse = 0, int yEllipse = 0) {
		return ::RoundRect(hdc, left, top, right, bottom, xEllipse, yEllipse);
	}
	inline bool roundRect(const ::HDC& hdc, const ::RECT& ltrb,
						  int xEllipse = 0, int yEllipse = 0) {
		return ::RoundRect(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, xEllipse, yEllipse);
	}
	inline bool roundRect(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& cornerEllipseSize = Vec2f::s_zero()) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return roundRect(hdc, ltrb, static_cast<int>(cornerEllipseSize.x), static_cast<int>(cornerEllipseSize.y));
	}

	inline bool arc(const ::HDC& hdc,
					int left, int top, int right, int bottom,
					int fromX, int fromY, int toX, int toY) {
		return ::Arc(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool arc(const ::HDC& hdc, const ::RECT& ltrb,
					int fromX, int fromY, int toX, int toY) {
		return ::Arc(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool arc(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return arc(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool chord(const ::HDC& hdc,
					  int left,  int top,   int right, int bottom,
					  int fromX, int fromY, int toX,   int toY) {
		return ::Chord(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool chord(const ::HDC& hdc, const ::RECT& ltrb,
					  int fromX, int fromY, int toX, int toY) {
		return ::Chord(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool chord(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return chord(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool pie(const ::HDC& hdc,
					int left,  int top,   int right, int bottom,
					int fromX, int fromY, int toX,   int toY) {
		return ::Pie(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool pie(const ::HDC& hdc, const ::RECT& ltrb,
					int fromX, int fromY, int toX, int toY) {
		return ::Pie(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool pie(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return pie(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool lineTo(const ::HDC& hdc, int x, int y) { return ::LineTo(hdc, x, y); }
	inline bool lineTo(const ::HDC& hdc, const ::POINT& pt)	{ return ::LineTo(hdc, pt.x, pt.y); }
	inline bool lineTo(const ::HDC& hdc, const Vec2f& pt) {
		return ::LineTo(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y));
	}
	inline bool lineTo(const ::HDC& hdc, const Vec2i& pt) { return ::LineTo(hdc, pt.x, pt.y); }

	inline void drawLine(const ::HDC& hdc, int fromX, int fromY, int toX, int toY) {
		::POINT lastPt;
		::MoveToEx(hdc, fromX, fromY, &lastPt);
		::LineTo(hdc, toX, toY);
		::MoveToEx(hdc, lastPt.x, lastPt.y, nullptr);
	}
	inline void drawLine(const ::HDC& hdc, const ::POINT& from, const ::POINT& to) {
		drawLine(hdc, from.x, from.y, to.x, to.y);
	}
	inline void drawLine(const ::HDC& hdc, const Vec2i& from, const Vec2i& to) {
		drawLine(hdc, from.x, from.y, to.x, to.y);
	}
	inline void drawLine(const ::HDC& hdc, const Vec2f& from, const Vec2f& to) {
		auto iFrom = Vec2i::s_cast(from);
		auto iTo = Vec2i::s_cast(to);
		drawLine(hdc, iFrom, iTo);
	}

	int fillRect(const ::HDC& hdc, const Rect2f& xywh, const Color4b& c);
	int fillRect(const ::HDC& hdc, const Rect2f& xywh, StockObj_Brush flag);

} // namespace GDI
} // namespace sge

#endif