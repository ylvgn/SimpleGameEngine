#pragma once

#if SGE_OS_WINDOWS

#include "PW5_Common.h"
#include <sge_core/native_ui/win32/Win32Util.h>

#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

namespace sge {

inline
WNDCLASSEX g_createWndClass(HMODULE hInstance, const wchar_t* clsName, WNDPROC lpfnWndProc = nullptr) {
	WNDCLASSEX wc;
	g_bzero(wc);

	wc.cbSize			= sizeof(WNDCLASSEX);
    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= lpfnWndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(hInstance, IDC_ARROW);
    wc.hbrBackground	= nullptr;
    wc.lpszMenuName		= nullptr;
    wc.lpszClassName	= clsName;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	return wc;
}

inline
bool g_isRegisterWndClass(HMODULE hInstance, const wchar_t* clsName) {
	WNDCLASSEX wc;
	return 0 != GetClassInfoEx(hInstance, clsName, &wc);
}

inline
void g_registerWndClass(WNDCLASSEX& wc) {
	if (g_isRegisterWndClass(wc.hInstance, wc.lpszClassName))
		return;

	if (!RegisterClassEx(&wc)) {
		DWORD e = GetLastError();
		switch (e) {
			case ERROR_CALL_NOT_IMPLEMENTED: throw SGE_ERROR("calling RegisterClassW in Windows 98");
			default: throw SGE_ERROR("error g_registerWndClass");
		}
	}
}

inline
void g_textOut(HDC hdc, int x, int y, const wchar_t* szText) {
	if (!szText) return;
	TextOut(hdc, x, y, szText, lstrlenW(szText) /*or static_cast<int>(wcslen(text))*/);
}

inline
void g_textOutf(HDC hdc, int x, int y, const wchar_t* szFormat, ...) {
	wchar_t szBuffer[220];
	va_list args;
	va_start(args, szFormat);
	size_t bufferCount = sizeof(szBuffer) / sizeof(wchar_t);
	auto len = _vsntprintf_s(szBuffer, bufferCount, szFormat, args);
	va_end(args);
	TextOut(hdc, x, y, szBuffer, len);
}

inline
void g_rectangle(HDC hdc, const Rect2i& rc) {
	int l = rc.x;
	int t = rc.y;
	int r = rc.x + rc.w;
	int b = rc.y + rc.h;
	Rectangle(hdc, l, t, r, b);
}

inline
void g_rectangle(HDC hdc, const RECT& rc) {
	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
}

inline
void g_drawText(HDC				hdc,
				RECT*			rc,
				const wchar_t*	szText,
				u32				fDT = DT_SINGLELINE | DT_CENTER | DT_VCENTER)
{
	if (!szText || !rc) return;
	DrawText(hdc, szText, -1, rc, fDT);
}


class ScopedHDCBase : public NonCopyable {
public:
	ScopedHDCBase(HWND& hwnd) : _hwnd(hwnd) , _hdc(nullptr) {}
	virtual ~ScopedHDCBase() = default;

	void textOut(int x, int y, const wchar_t* szText) const { g_textOut(_hdc, x, y, szText); }

	template<class... Args>
	void textOutf(int x, int y, Args&&... args) const {
		TempStringW tmp;
		FmtTo(tmp, SGE_FORWARD(args)...);
		TextOut(_hdc, x, y, tmp.c_str(), static_cast<int>(tmp.size()));
	}

	void rectangle(int left, int top, int right, int bottom) const { Rectangle(_hdc, left, top, right, bottom); }
	void rectangle(const RECT& rc) const	{ g_rectangle(_hdc, rc); }
	void rectangle(const Rect2i& rc) const	{ g_rectangle(_hdc, rc); }

	TEXTMETRIC getTextMetrics() { TEXTMETRIC tm; GetTextMetrics(_hdc, &tm); return tm; }

	operator const HDC& () const { return _hdc; }

protected:
	HWND&	_hwnd;
	HDC		_hdc;
};

class ScopedPaintStruct : public ScopedHDCBase {
	using Base = ScopedHDCBase;
public:
	ScopedPaintStruct(HWND& hwnd) : Base(hwnd) { _hdc = BeginPaint(hwnd, &_ps); }
	~ScopedPaintStruct() { EndPaint(_hwnd, &_ps); }

	BOOL fErase()			const { return _ps.fErase; }
	const RECT& rcPaint()	const { return _ps.rcPaint; }

	const PAINTSTRUCT& get() const { return _ps; }

private:
	PAINTSTRUCT _ps;
};

class ScopedHDC : public ScopedHDCBase {
	using Base = ScopedHDCBase;
public:
	ScopedHDC(HWND& hwnd) : Base(hwnd) { _hdc = GetDC(hwnd); }
	~ScopedHDC() { ReleaseDC(_hwnd, _hdc); }
};

class MyTextMetric : public NonCopyable {

	void _init(const TEXTMETRIC& tm) {
		_height					= tm.tmHeight;
		_ascent					= tm.tmAscent;
		_descent				= tm.tmDescent;
		_internalLeading		= tm.tmInternalLeading;
		_maxCharWidth			= tm.tmMaxCharWidth;
		_externalLeading		= tm.tmExternalLeading;
		_aveCharWidth			= tm.tmAveCharWidth;

		_isFixedPitch			= (tm.tmPitchAndFamily & 1) == 0;
		_aveCharHeight			= _height + _externalLeading;
		_aveUpperCaseCharHeight = _isFixedPitch ? _aveCharWidth : static_cast<int>(1.5f * _aveCharWidth);
	}

public:
	MyTextMetric(HWND hwnd) {
		ScopedHDC hdc(hwnd);
		auto tm = hdc.getTextMetrics();
		_init(tm);
	}

	MyTextMetric(HDC hdc) {
		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);
		_init(tm);
	}

	MyTextMetric(const TEXTMETRIC& tm) {
		_init(tm);
	}

	int		height()				const	{ return _height; }
	int		ascent()				const	{ return _ascent; }
	int		descent()				const	{ return _descent; }
	int		internalLeading()		const	{ return _internalLeading; }
	int		maxCharWidth()			const	{ return _maxCharWidth; }
	int		externalLeading()		const	{ return _externalLeading; }
	int		aveCharWidth()			const	{ return _aveCharWidth; }
	int		aveCharHeight()			const	{ return _aveCharHeight; }
	int		aveUpperCaseCharWidth()	const	{ return _aveUpperCaseCharHeight; }
	bool	isFixedPitch()			const	{ return _isFixedPitch; }
	bool	isVariableWidth()		const	{ return !_isFixedPitch; }

private:

	int _height;
	int _ascent;
	int _descent;
	int _internalLeading;
	int _maxCharWidth;
	int _externalLeading;
	int _aveCharWidth;

	bool _isFixedPitch : 1;
	int _aveCharHeight;
	int _aveUpperCaseCharHeight;
};

}

#endif // SGE_OS_WINDOWS