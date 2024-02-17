#pragma once
#if SGE_OS_WINDOWS

#include "PW5_Common.h"

namespace sge {
namespace GDI {

	static constexpr u32 kDrawTextFlagDefault = DT_SINGLELINE | DT_CENTER | DT_VCENTER;

	inline void textOut(HDC hdc, int x, int y, const wchar_t* szText) {
		if (!szText) return;
		::TextOut(hdc, x, y, szText, lstrlenW(szText) /*or static_cast<int>(wcslen(text))*/);
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

	inline bool rectangle(HDC hdc, const Rect2i& xywh) {
		int halfW = static_cast<int>(xywh.w * 0.5);
		int halfH = static_cast<int>(xywh.h * 0.5);
		int l = xywh.x - halfW;
		int t = xywh.y - halfH;
		int r = xywh.x + halfW;
		int b = xywh.y + halfH;
		return ::Rectangle(hdc, l, t, r, b);
	}
	inline bool rectangle(HDC hdc, int left, int top, int right, int bottom) {
		return ::Rectangle(hdc, left, top, right, bottom);
	}

	inline bool ellipse(HDC hdc, int left, int top, int right, int bottom) { return ::Ellipse(hdc, left, top, right, bottom); }
	inline bool ellipse(HDC hdc, const Rect2i& xywh) {
		int halfW = static_cast<int>(xywh.w * 0.5);
		int halfH = static_cast<int>(xywh.h * 0.5);
		int l = xywh.x - halfW;
		int t = xywh.y - halfH;
		int r = xywh.x + halfW;
		int b = xywh.y + halfH;
		return ::Ellipse(hdc, l, t, r, b);
	}

	inline bool roundRect(HDC hdc, int left, int top, int right, int bottom, int xCornerEllipse = 0, int yCornerEllipse = 0) {
		return ::RoundRect(hdc, left, top, right, bottom, xCornerEllipse, yCornerEllipse);
	}
	inline bool roundRect(HDC hdc, const Rect2i& xywh, int xCornerEllipse = 0, int yCornerEllipse = 0) {
		int halfW = static_cast<int>(xywh.w * 0.5);
		int halfH = static_cast<int>(xywh.h * 0.5);
		int l = xywh.x - halfW;
		int t = xywh.y - halfH;
		int r = xywh.x + halfW;
		int b = xywh.y + halfH;
		return ::RoundRect(hdc, l, t, r, b, xCornerEllipse, yCornerEllipse);
	}

	inline bool arc(HDC hdc, int left, int top, int right, int bottom, int xStart, int yStart, int xEnd, int yEnd) {
		return ::Arc(hdc, left, top, right, bottom, xStart, yStart, xEnd, yEnd);
	}

	inline bool chord(HDC hdc, int left, int top, int right, int bottom, int xStart, int yStart, int xEnd, int yEnd) {
		return ::Chord(hdc, left, top, right, bottom, xStart, yStart, xEnd, yEnd);
	}

	inline bool pie(HDC hdc, int left, int top, int right, int bottom, int xStart, int yStart, int xEnd, int yEnd) {
		return ::Pie(hdc, left, top, right, bottom, xStart, yStart, xEnd, yEnd);
	}

	inline bool lineTo(HDC hdc, const Vec2i& pt)	{ return ::LineTo(hdc, pt.x, pt.y); }
	inline bool lineTo(HDC hdc, int x, int y)		{ return ::LineTo(hdc, x, y); }
	inline bool lineTo(HDC hdc, const ::POINT& pt)	{ return ::LineTo(hdc, pt.x, pt.y); }

	inline void drawText(HDC			hdc,
						::RECT*			ltrb,
						const wchar_t*	szText,
						u32				fDT = kDrawTextFlagDefault)
	{
		if (!szText || !ltrb) return;
		::DrawText(hdc, szText, -1, ltrb, fDT);
	}

	inline void drawLine(HDC hdc, int fromX, int fromY, int toX, int toY) {
		::POINT lastPt;
		::MoveToEx(hdc, fromX, fromY, &lastPt);
		::LineTo(hdc, toX, toY);
		::MoveToEx(hdc, lastPt.x, lastPt.y, nullptr);
	}

	inline void drawLine(HDC hdc, const ::POINT& from, const ::POINT& to)	{ drawLine(hdc, from.x, from.y, to.x, to.y); }
	inline void drawLine(HDC hdc, const Vec2i& from, const Vec2i& to)		{ drawLine(hdc, from.x, from.y, to.x, to.y); }

	inline ::COLORREF COLORREF_make(int r, int g, int b)	{ return RGB(r, g, b); }
	inline ::COLORREF COLORREF_make(const Color4b& c)		{ return RGB(c.r, c.g, c.b); }

	inline ::HBRUSH createSolidBrush(int r, int g, int b) { return ::CreateSolidBrush(COLORREF_make(r, g, b)); }

	inline void fillRect(HDC hdc, const ::RECT& rc, const ::HBRUSH& brush) {
		::SelectObject(hdc, brush);
		::FillRect(hdc, &rc, brush);
		::SelectObject(hdc, nullptr);
	}

} // namespace GDI

#define TextAlignmentOption_ENUM_LIST(E) \
	E(Left,) \
	E(Right,) \
	E(Top,) \
	E(Bottom,) \
	E(Center,) \
	E(BaseLine,) \
//----
SGE_ENUM_CLASS(TextAlignmentOption, u8)
SGE_ENUM_ALL_OPERATOR(TextAlignmentOption)

#define DrawTextFormatFlag_ENUM_LIST(E) \
	E(Top,) \
	E(Left,) \
	E(Center,) \
	E(Right,) \
	E(VCenter,) \
	E(Bottom,) \
	E(WordBreak,) \
	E(SingleLine,) \
	E(EXPANDTABS,) \
	E(TabStop,) \
	E(NoClip,) \
	E(EXTERNALLEADING,) \
	E(CALCRECT,) \
	E(NOPREFIX,) \
	E(INTERNAL,) \
//----
SGE_ENUM_CLASS(DrawTextFormatFlag, u8)
SGE_ENUM_ALL_OPERATOR(DrawTextFormatFlag)

#define StockLogicalObject_ENUM_LIST(E) \
	E(None,) \
	E(White,) \
	E(LtGray,) \
	E(Gray,) \
	E(DkGray,) \
	E(Black,) \
//----
SGE_ENUM_CLASS(StockLogicalObject, u8)

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
#pragma mark ========= HDC(handle device context) ============
#endif
class MyHDC_NoHWND : public NonCopyable {
public:
	virtual ~MyHDC_NoHWND() = default;

	void textOut(int x, int y, const wchar_t* szText) const { GDI::textOut(_hdc, x, y, szText); }

	template<class... Args>
	void Fmt_textOut(int x, int y, Args&&... args) const {
		auto s = Fmt(SGE_FORWARD(args)...);
		auto ws = UtfUtil::toStringW(s);
		::TextOut(_hdc, x, y, ws.c_str(), static_cast<int>(ws.size()));
	}

	UINT setTextAlign(TextAlignmentOption flags = TextAlignmentOption::Left | TextAlignmentOption::Top);

	inline void drawText(::RECT* rc,
						 const wchar_t* szText,
						 u32	fDT = GDI::kDrawTextFlagDefault)
	{
		GDI::drawText(_hdc, rc, szText, fDT);
	}

	void rectangle(int left, int top, int right, int bottom)	const { GDI::rectangle(_hdc, left, top, right, bottom); }
	void rectangle(const Rect2i& xywh)							const { GDI::rectangle(_hdc, xywh); }

	void roundRect(int left, int top, int right, int bottom, int ellipseSize = 0) {
		GDI::roundRect(_hdc, left, top, right, bottom, ellipseSize, ellipseSize);
	}

	void drawLine(int fromX, int fromY, int toX, int toY)		const { GDI::drawLine(_hdc, fromX, fromY, toX, toY); }
	void drawLine(const ::POINT& from, const ::POINT& to)		const { GDI::drawLine(_hdc, from, to); }
	void drawLine(const Vec2i& from, const Vec2i& to)			const { GDI::drawLine(_hdc, from, to); }

	void ellipse(int left, int top, int right, int bottom)		const { GDI::ellipse(_hdc, left, top, right, bottom); }

	TextMetrics createTextMetrics() { return TextMetrics(_hdc); }

	operator const HDC& () const { return _hdc; }

protected:
	HDC	_hdc = nullptr;
};

class MyHDC : public MyHDC_NoHWND {
	using Base = MyHDC_NoHWND;
public:
	MyHDC(HWND& hwnd)
		: _hwnd(hwnd) {}

	void clearBg(StockLogicalObject flag = StockLogicalObject::White);

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

}

#endif