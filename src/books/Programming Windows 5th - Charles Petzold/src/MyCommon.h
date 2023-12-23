#pragma once

#include <sge_core.h>

#if SGE_OS_WINDOWS

// WINAPI/CALLBACK -> __stdcall function calling convention
	// involves how machine code is generated to place function call arguments on the stack

// window main entry
	// Win32: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow);
		// HINSTANCE -> instance handle, is simply a number, to uniquely identifies the program.
		// hPrevInstance is always NULL in 32 bit versions of Windows
	// Win16: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
		// LPSTR == PSTR -> char* (LPSTR is an artifact of 16 bit Windows),
			// 'NP' -> near pointer, 'LP' -> long pointer
			// There is no differentiation between near and long pointers in Win32, but two different sizes in Win16.

// window procedure
	// Win32: LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		// WPARAM is UINT, LPARAM is LONG, and both are 32 bit value
		// LRESULT is LONG
	// Win16: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam)
		// WPARAM is WORD, unsigned short int (16 bit) 
		// LPARAM is LONG, signed long int (32 bit)

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdint.h>
#include <tchar.h>

namespace sge {

template<class T> constexpr
void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

inline
WNDCLASSEX g_createWndClass(const wchar_t* clsName, WNDPROC lpfnWndProc = nullptr) {
	WNDCLASSEX wc;
	g_bzero(wc);

	HMODULE hInstance	= GetModuleHandle(nullptr);
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

	const HDC& hdc() const { return _hdc; }
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
public:
	MyTextMetric(HWND hwnd) {
		ScopedHDC hdc(hwnd);
		TEXTMETRIC tm			= hdc.getTextMetrics();

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