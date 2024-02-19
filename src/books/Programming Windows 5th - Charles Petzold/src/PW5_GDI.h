#pragma once
#if SGE_OS_WINDOWS

#include "PW5_Common.h"

namespace sge {

#define PW5_TextAlignmentOption_ENUM_LIST(E) \
	E(Left,)		\
	E(Right,)		\
	E(Top,)			\
	E(Bottom,)		\
	E(Center,)		\
	E(BaseLine,)	\
//----
SGE_ENUM_CLASS(PW5_TextAlignmentOption, u8)
SGE_ENUM_ALL_OPERATOR(PW5_TextAlignmentOption)

#define PW5_DrawTextFormatFlag_ENUM_LIST(E) \
	E(Top,				= 0)		\
	E(Left,				= 1 << 0)	\
	E(Center,			= 1 << 1)	\
	E(Right,			= 1 << 2)	\
	E(VCenter,			= 1 << 3)	\
	E(Bottom,			= 1 << 4)	\
	E(WordBreak,		= 1 << 5)	\
	E(SingleLine,		= 1 << 6)	\
	E(ExpandTabs,		= 1 << 7)	\
	E(TabStop,			= 1 << 8)	\
	E(NoClip,			= 1 << 9)	\
	E(ExternalLeading,	= 1 << 10)	\
	E(CalcRect,			= 1 << 11)	\
	E(NoPrefix,			= 1 << 12)	\
//----
SGE_ENUM_CLASS(PW5_DrawTextFormatFlag, u16)
SGE_ENUM_ALL_OPERATOR(PW5_DrawTextFormatFlag)

#define PW5_StockLogicalObject_Brush_ENUM_LIST(E) \
	E(None,)	\
	E(White,)	\
	E(LtGray,)	\
	E(Gray,)	\
	E(DkGray,)	\
	E(Black,)	\
//----
SGE_ENUM_CLASS(PW5_StockLogicalObject_Brush, u8)

#define PW5_StockLogicalObject_Pen_ENUM_LIST(E) \
	E(None,)	\
	E(White,)	\
	E(Black,)	\
//----
SGE_ENUM_CLASS(PW5_StockLogicalObject_Pen, u8)

#define PW5_PenStyle_ENUM_LIST(E) \
	E(None,)		\
	E(Solid,)		\
	E(Dash,)		\
	E(Dot,)			\
	E(DashDot,)		\
	E(DashDotDot,)	\
//----
SGE_ENUM_CLASS(PW5_PenStyle, u8)

} // namespace sge

namespace sge {
namespace GDI {

	using DTFlag = PW5_DrawTextFormatFlag;

	static constexpr Color4f kWhite		{ 1,1,1,1 };
	static constexpr Color4f kBlack		{ 0,0,0,1 };
	static constexpr Color4f kRed		{ 1,0,0,1 };
	static constexpr Color4f kGreen		{ 0,1,0,1 };
	static constexpr Color4f kBlue		{ 0,0,1,1 };
	static constexpr Color4f kYellow	{ 1,1,0,1 };
	static constexpr Color4f kViolet	{ 1,0,1,1 };
	static constexpr Color4f kCyan		{ 0,1,1,1 };

	inline void textOut(HDC hdc, int x, int y, StrView str) {
		if (str.size() == 0) return;
		auto s = UtfUtil::toStringW(str);
		::TextOut(hdc, x, y, s.c_str(), static_cast<int>(s.size()));
	}

	inline void textOutf(HDC hdc, int x, int y, const wchar_t* szFormat, ...) {
		wchar_t szBuffer[220];
		va_list args;

		va_start(args, szFormat);
		size_t bufferCount = sizeof(szBuffer) / sizeof(wchar_t);
		auto len = _vsntprintf_s(szBuffer, bufferCount, szFormat, args);
		va_end(args);

		::TextOut(hdc, x, y, szBuffer, len);
	}

	void drawText(HDC hdc, int left, int top, int right, int bottom, StrView str, DTFlag flags);

	inline void drawText(HDC hdc,
						 int left, int top, int right, int bottom,
						 StrView str,
						 UINT fDT = DT_SINGLELINE | DT_CENTER | DT_VCENTER)
	{
		if (str.size() == 0) return;
		::RECT ltrb { left, top, right, bottom };
		auto s = UtfUtil::toStringW(str);
		::DrawText(hdc, s.c_str(), static_cast<int>(str.size()), &ltrb, fDT);
	}

	inline void drawText(HDC hdc, const ::RECT& ltrb, StrView str, DTFlag flags) {
		drawText(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, str, flags);
	}
	inline void drawText(HDC hdc, const Rect2f& xywh, StrView str, DTFlag flags) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return drawText(hdc, ltrb, str, flags);
	}

	inline bool rectangle(HDC hdc, int left, int top, int right, int bottom) {
		return ::Rectangle(hdc, left, top, right, bottom);
	}
	inline bool rectangle(HDC hdc, const ::RECT& ltrb) {
		return ::Rectangle(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom);
	}
	inline bool rectangle(HDC hdc, const Rect2f& xywh) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return rectangle(hdc, ltrb);
	}

	inline bool ellipse(HDC hdc, int left, int top, int right, int bottom) { return ::Ellipse(hdc, left, top, right, bottom); }
	inline bool ellipse(HDC hdc, const ::RECT& ltrb) {
		return ::Ellipse(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom);
	}
	inline bool ellipse(HDC hdc, const Rect2f& xywh) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return ellipse(hdc, ltrb);
	}

	inline bool roundRect(HDC hdc, int left, int top, int right, int bottom, int xEllipse = 0, int yEllipse = 0) {
		return ::RoundRect(hdc, left, top, right, bottom, xEllipse, yEllipse);
	}
	inline bool roundRect(HDC hdc, const ::RECT& ltrb, int xEllipse = 0, int yEllipse = 0) {
		return ::RoundRect(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, xEllipse, yEllipse);
	}
	inline bool roundRect(HDC hdc, const Rect2f& xywh, const Vec2f& cornerEllipseSize = {0,0}) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return roundRect(hdc, ltrb, static_cast<int>(cornerEllipseSize.x), static_cast<int>(cornerEllipseSize.y));
	}

	inline bool arc(HDC hdc,
					int left, int top, int right, int bottom,
					int fromX, int fromY, int toX, int toY)
	{
		return ::Arc(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool arc(HDC hdc, const ::RECT& ltrb,
					int fromX, int fromY, int toX, int toY)
	{
		return ::Arc(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool arc(HDC hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return arc(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool chord(	HDC hdc,
						int left, int top, int right, int bottom,
						int fromX, int fromY, int toX, int toY)
	{
		return ::Chord(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool chord(	HDC hdc, const ::RECT& ltrb,
						int fromX, int fromY, int toX, int toY)
	{
		return ::Chord(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool chord(HDC hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return chord(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool pie(HDC hdc,
					int left, int top, int right, int bottom,
					int fromX, int fromY, int toX, int toY)
	{
		return ::Pie(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool pie(HDC hdc, const ::RECT& ltrb,
					int fromX, int fromY, int toX, int toY)
	{
		return ::Pie(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool pie(HDC hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return pie(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool lineTo(HDC hdc, int x, int y)		{ return ::LineTo(hdc, x, y); }
	inline bool lineTo(HDC hdc, const ::POINT& pt)	{ return ::LineTo(hdc, pt.x, pt.y); }
	inline bool lineTo(HDC hdc, const Vec2f& pt) {
		::POINT _pt;
		Win32Util::convert(_pt, pt);
		return lineTo(hdc, _pt);
	}

	inline void drawLine(HDC hdc, int fromX, int fromY, int toX, int toY) {
		::POINT lastPt;
		::MoveToEx(hdc, fromX, fromY, &lastPt);
		::LineTo(hdc, toX, toY);
		::MoveToEx(hdc, lastPt.x, lastPt.y, nullptr);
	}

	inline void drawLine(HDC hdc, const ::POINT& from, const ::POINT& to) { drawLine(hdc, from.x, from.y, to.x, to.y); }
	inline void drawLine(HDC hdc, const Vec2f& from, const Vec2f& to) {
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		drawLine(hdc, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline ::COLORREF COLORREF_make(int r, int g, int b)	{ return RGB(r, g, b); }
	inline ::COLORREF COLORREF_make(const Color4b& c)		{ return RGB(c.r, c.g, c.b); }
	inline ::COLORREF COLORREF_make(const Color4f& c)		{ return RGB(c.r*255, c.g*255, c.b*255); }

	inline ::HBRUSH createSolidBrush(int r, int g, int b)	{ return ::CreateSolidBrush(COLORREF_make(r, g, b)); }
	inline ::HBRUSH createSolidBrush(const ::COLORREF& c)	{ return ::CreateSolidBrush(c); }
	inline ::HBRUSH createSolidBrush(const Color4b& c)		{ return ::CreateSolidBrush(COLORREF_make(c)); }
	inline ::HBRUSH createSolidBrush(const Color4f& c)		{ return ::CreateSolidBrush(COLORREF_make(c)); }

	inline void fillRect(HDC hdc, const ::RECT& rc, const ::HBRUSH& brush) {
		::SelectObject(hdc, brush);
		::FillRect(hdc, &rc, brush);
		::SelectObject(hdc, nullptr);
	}

	void drawDashedLine(HDC hdc, int fromX, int fromY, int toX, int toY, const Color4f& c = kBlack);
	void drawDashedLine(HDC hdc, const Vec2f& from, const Vec2f& to, const Color4f& c = kBlack);

} // namespace GDI
} // namespace sge


namespace sge {
#if 0
#pragma mark ========= TextMetrics ============
#endif
struct TextMetrics {
	TextMetrics(HWND hwnd);
	TextMetrics(HDC hdc);
	TextMetrics(const ::TEXTMETRIC& tm);

	int		height;
	int		ascent;
	int		descent;
	int		internalLeading;
	int		maxCharWidth;
	int		externalLeading;
	int		aveCharWidth;
	int		aveCharHeight;
	int		aveCharWidthUpperCase;
	bool	isFixedPitch : 1;

private:
	void	_internal_init(HDC hdc);
	void	_set(const ::TEXTMETRIC& tm);
};

#if 0
#pragma mark ========= HDC ============
#endif
class MyHDC_NoHWND : public NonCopyable {
public:
	using DTFlag = GDI::DTFlag;

	virtual ~MyHDC_NoHWND() = default;

	operator const HDC& () const { return _hdc; }

	TextMetrics createTextMetrics() const { return TextMetrics(_hdc); }

	UINT setTextAlign(PW5_TextAlignmentOption flags = PW5_TextAlignmentOption::Left | PW5_TextAlignmentOption::Top);

	void textOut(int x, int y, StrView str) const { GDI::textOut(_hdc, x, y, str); }

	void drawText(int l, int t, int r, int b, StrView str)					const { GDI::drawText(_hdc, l, t, r, b, str); }
	void drawText(int l, int t, int r, int b, StrView str, DTFlag flags)	const { GDI::drawText(_hdc, l, t, r, b, str, flags); }
	void drawText(const ::RECT& ltrb, StrView str, DTFlag flags)			const { GDI::drawText(_hdc, ltrb, str, flags); }
	void drawText(const Rect2f& xywh, StrView str, DTFlag flags)			const { GDI::drawText(_hdc, xywh, str, flags); }

	void drawLine(int fromX, int fromY, int toX, int toY)					const { GDI::drawLine(_hdc, fromX, fromY, toX, toY); }
	void drawLine(const ::POINT& from, const ::POINT& to)					const { GDI::drawLine(_hdc, from, to); }
	void drawLine(const Vec2f& from, const Vec2f& to)						const { GDI::drawLine(_hdc, from, to); }

	void rectangle(int left, int top, int right, int bottom)				const { GDI::rectangle(_hdc, left, top, right, bottom); }
	void rectangle(const ::RECT& ltrb)										const { GDI::rectangle(_hdc, ltrb); }
	void rectangle(const Rect2f& xywh)										const { GDI::rectangle(_hdc, xywh); }

	void ellipse(int left, int top, int right, int bottom)					const { GDI::ellipse(_hdc, left, top, right, bottom); }
	void ellipse(const ::RECT& ltrb)										const { GDI::ellipse(_hdc, ltrb); }
	void ellipse(const Rect2f& xywh)										const { GDI::ellipse(_hdc, xywh); }

	void roundRect(	int left, int top, int right, int bottom,
					int xCornerEllipse = 0, int yCornerEllipse = 0)			const { GDI::roundRect(_hdc, left, top, right, bottom, xCornerEllipse, yCornerEllipse); }
	void roundRect(const ::RECT& ltrb, int xEllipse = 0, int yEllipse = 0)	const { GDI::roundRect(_hdc, ltrb, xEllipse, yEllipse); }
	void roundRect(const Rect2f& xywh, const Vec2f& cornerEllipseSize)		const { GDI::roundRect(_hdc, xywh, cornerEllipseSize); }

	void arc(	int left, int top, int right, int bottom,
				int fromX, int fromY, int toX, int toY)						const { GDI::arc(_hdc, left, top, right, bottom, fromX, fromY, toX, toY); }
	void arc(const ::RECT& ltrb, int fromX, int fromY, int toX, int toY)	const { GDI::arc(_hdc, ltrb, fromX, fromY, toX, toY); }
	void arc(const Rect2f& xywh, const Vec2f& from, const Vec2f& to)		const { GDI::arc(_hdc, xywh, from, to); }

	void chord(	int left, int top, int right, int bottom,
				int fromX, int fromY, int toX, int toY)						const { GDI::chord(_hdc, left, top, right, bottom, fromX, fromY, toX, toY); }
	void chord(const ::RECT& ltrb, int fromX, int fromY, int toX, int toY)	const { GDI::chord(_hdc, ltrb, fromX, fromY, toX, toY); }
	void chord(const Rect2f& xywh, const Vec2f& from, const Vec2f& to)		const { GDI::chord(_hdc, xywh, from, to); }
	
	void pie(	int left, int top, int right, int bottom,
				int fromX, int fromY, int toX, int toY)						const { GDI::pie(_hdc, left, top, right, bottom, fromX, fromY, toX, toY); }
	void pie(const ::RECT& ltrb, int fromX, int fromY, int toX, int toY)	const { GDI::pie(_hdc, ltrb, fromX, fromY, toX, toY); }
	void pie(const Rect2f& xywh, const Vec2f& from, const Vec2f& to)		const { GDI::pie(_hdc, xywh, from, to); }

	template<class... Args>
	void Fmt_textOut(int x, int y, Args&&... args) const {
		auto tmpStr = Fmt(SGE_FORWARD(args)...);
		auto s = UtfUtil::toStringW(tmpStr);
		::TextOut(_hdc, x, y, s.c_str(), static_cast<int>(s.size()));
	}

	template<class... Args>
	void Fmt_drawText(int x, int y, Args&&... args) const {
		auto s = Fmt(SGE_FORWARD(args)...);
		auto tm = createTextMetrics();
		GDI::drawText(_hdc, x, y, x + static_cast<int>(s.size() * tm.maxCharWidth), y + tm.aveCharHeight, s.view(), 0);
	}

protected:
	HDC	_hdc = nullptr;
};

class MyHDC : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	MyHDC(HWND& hwnd)
		: _hwnd(hwnd) {}

	void clearBg(PW5_StockLogicalObject_Brush flag = PW5_StockLogicalObject_Brush::White);

protected:
	HWND&	_hwnd;
};

#if 0
#pragma mark ========= Scoped HDC ============
#endif
class ScopedPaintStruct : public MyHDC {
	using Base = MyHDC;
public:
	ScopedPaintStruct(HWND& hwnd) : Base(hwnd) {
		_hdc = ::BeginPaint(hwnd, &_ps);
	}
	~ScopedPaintStruct() { ::EndPaint(_hwnd, &_ps); }

			HDC		hdc()		const { return _ps.hdc;		}
			bool	fErase()	const { return _ps.fErase;	}
	const	::RECT&	rcPaint()	const { return _ps.rcPaint; }

private:
	::PAINTSTRUCT _ps;
};

class ScopedGetDC : public MyHDC {
	using Base = MyHDC;
public:
	ScopedGetDC(HWND& hwnd) : Base(hwnd) {
		_hdc = ::GetDC(hwnd);
	}
	~ScopedGetDC() { ::ReleaseDC(_hwnd, _hdc); }
};

class ScopedGetWindowDC : public MyHDC {
	using Base = MyHDC;
public:
	ScopedGetWindowDC(HWND& hwnd) : Base(hwnd) {
		_hdc = ::GetWindowDC(hwnd);
	}
	~ScopedGetWindowDC() { ::ReleaseDC(_hwnd, _hdc); }
};

class ScopedCreateDC : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	ScopedCreateDC( const wchar_t* pszDriver,
					const wchar_t* pszDevice = nullptr,
					const wchar_t* pszOutput = nullptr,
					const ::DEVMODEW* pData  = nullptr)
	{
		_hdc = ::CreateDC(pszDriver, pszDevice, pszOutput, pData);
	}
	~ScopedCreateDC() { ::DeleteDC(_hdc); }
};

class ScopedCreateIC : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	ScopedCreateIC( const wchar_t* pszDriver,
					const wchar_t* pszDevice = nullptr,
					const wchar_t* pszOutput = nullptr,
					const ::DEVMODEW* pData = nullptr)
	{
		_hdc = ::CreateIC(pszDriver, pszDevice, pszOutput, pData);
	}
	~ScopedCreateIC() { ::DeleteDC(_hdc); }
};

class ScopedCreateCompatibleDC : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	ScopedCreateCompatibleDC(HDC srcHdc) {
		_hdc = ::CreateCompatibleDC(srcHdc);
	}
	~ScopedCreateCompatibleDC() { ::DeleteDC(_hdc); }
};

class ScopedSaveDC : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	ScopedSaveDC(HDC srcHdc) {
		_hdc = srcHdc;
		_id = ::SaveDC(srcHdc);
	}
	~ScopedSaveDC() { ::RestoreDC(_hdc, _id); } 
private:
	int _id;
};

class ScopedCreateSolidBrush : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	ScopedCreateSolidBrush(HDC hdc, const Color4f& c = GDI::kWhite) {
		_brush = GDI::createSolidBrush(GDI::COLORREF_make(c));
		SelectBrush(hdc, _brush);
	}
	~ScopedCreateSolidBrush() { DeleteBrush(_brush); }
private:
	HBRUSH _brush;
};

class ScopedExtCreatePen : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	ScopedExtCreatePen(HDC hdc, const LOGBRUSH& brush, PW5_PenStyle flag = PW5_PenStyle::Solid);
	~ScopedExtCreatePen();
private:
	HPEN _pen;
};

}

#endif