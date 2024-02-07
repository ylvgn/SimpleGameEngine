#pragma once

#if SGE_OS_WINDOWS

#include "PW5_Common.h"

namespace sge {

namespace GDI {

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

		TextOut(hdc, x, y, szBuffer, len);
	}

	inline bool rectangle(HDC hdc, const Rect2i& rc) {
		int l = rc.x;
		int t = rc.y;
		int r = rc.x + rc.w;
		int b = rc.y + rc.h;
		return ::Rectangle(hdc, l, t, r, b);
	}

	inline bool rectangle(HDC hdc, const RECT& rc) {
		return ::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
	}

	inline bool lineTo(HDC hdc, const Vec2i& pt) {
		return ::LineTo(hdc, pt.x, pt.y);
	}

	inline void drawText(HDC			hdc,
						::RECT*			rc,
						const wchar_t*	szText,
						u32				fDT = DT_SINGLELINE | DT_CENTER | DT_VCENTER)
	{
		if (!szText || !rc) return;
		::DrawText(hdc, szText, -1, rc, fDT);
	}

} // namespace GDI

#define TextAlignment_ENUM_LIST(E) \
	E(Left,) \
	E(Right,) \
	E(Top,) \
	E(Bottom,) \
	E(Center,) \
	E(BaseLine,) \
//----
SGE_ENUM_CLASS(TextAlignment, u8)
SGE_ENUM_ALL_OPERATOR(TextAlignment)

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

#if 0
#pragma mark ========= TextMetrics ============
#endif
struct TextMetrics {
	TextMetrics(HWND hwnd);
	TextMetrics(HDC hdc);
	TextMetrics(const TEXTMETRIC& tm);

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
	void	_set(const TEXTMETRIC& tm);
};

class MyHDC : public NonCopyable {
public:
	MyHDC(HWND hwnd)
		: _hwnd(hwnd)
		, _hdc(nullptr) {}

	virtual ~MyHDC() = default;

	void textOut(int x, int y, const wchar_t* szText) const { GDI::textOut(_hdc, x, y, szText); }

	template<class... Args>
	void Fmt_textOut(int x, int y, Args&&... args) const {
		auto s = Fmt(SGE_FORWARD(args)...);
		auto ws = UtfUtil::toStringW(s);
		::TextOut(_hdc, x, y, ws.c_str(), static_cast<int>(ws.size()));
	}

	inline void drawText(::RECT* rc, const wchar_t* szText, u32	fDT = DT_SINGLELINE | DT_CENTER | DT_VCENTER) {
		GDI::drawText(_hdc, rc, szText, fDT);
	}

	UINT setTextAlign(TextAlignment align = TextAlignment::Left | TextAlignment::Top);

	void rectangle(int left, int top, int right, int bottom)	const { ::Rectangle(_hdc, left, top, right, bottom); }
	void rectangle(const ::RECT& rc)							const { GDI::rectangle(_hdc, rc); }
	void rectangle(const Rect2i& rc)							const { GDI::rectangle(_hdc, rc); }

	void lineTo(int x, int y)		const	{ GDI::lineTo(_hdc, {x,y}); }
	void lineTo(const ::POINT& pt)	const	{ GDI::lineTo(_hdc, Win32Util::toVec2i(pt)); }
	void lineTo(const Vec2i& pt)	const	{ GDI::lineTo(_hdc, pt); }

	TextMetrics createTextMetrics() { return TextMetrics(_hdc); }

	operator const HDC& () const { return _hdc; }

protected:
	HWND&	_hwnd;
	HDC		_hdc;
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
	ScopedGetWindowDC(HWND hwnd) : Base(hwnd) {
		_hdc = ::GetWindowDC(hwnd);
	}

	~ScopedGetWindowDC() { ::ReleaseDC(_hwnd, _hdc); }
};

class ScopedCreateDC : public MyHDC {
	using Base = MyHDC;
public:
	ScopedCreateDC( const wchar_t* pszDriver,
					const wchar_t* pszDevice = nullptr,
					const wchar_t* pszOutput = nullptr,
					const ::DEVMODEW* pData  = nullptr) : Base(nullptr)
	{
		_hdc = ::CreateDC(pszDriver, pszDevice, pszOutput, pData);
	}

	~ScopedCreateDC() { ::DeleteDC(_hdc); }
};

class ScopedCreateIC : public MyHDC {
	using Base = MyHDC;
public:
	ScopedCreateIC( const wchar_t* pszDriver,
					const wchar_t* pszDevice = nullptr,
					const wchar_t* pszOutput = nullptr,
					const ::DEVMODEW* pData = nullptr) : Base(nullptr)
	{
		_hdc = ::CreateIC(pszDriver, pszDevice, pszOutput, pData);
	}

	~ScopedCreateIC() { ::DeleteDC(_hdc); }
};

class ScopedCreateCompatibleDC : public MyHDC {
	using Base = MyHDC;
public:
	ScopedCreateCompatibleDC(HDC srcHdc) : Base(nullptr) {
		_hdc = ::CreateCompatibleDC(srcHdc);
	}

	~ScopedCreateCompatibleDC() { ::DeleteDC(_hdc); }
};

}

#endif