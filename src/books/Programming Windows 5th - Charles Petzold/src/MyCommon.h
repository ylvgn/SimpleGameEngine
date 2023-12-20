#pragma once

#include <sge_core.h>

#if SGE_OS_WINDOWS

// WINAPI/CALLBACK -> __stdcall function calling convention
	// involves how machine code is generated to place function call arguments on the stack

// main entry
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

constexpr const u32 kDefaultDT = DT_SINGLELINE | DT_CENTER | DT_VCENTER;

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
void g_drawText(HDC				hdc,
				RECT*			pos,
				const wchar_t*	szText,
				u32				fDT = DT_SINGLELINE | DT_CENTER | DT_VCENTER)
{
	if (!szText) return;
	DrawText(hdc, szText, -1, pos, fDT);
}


class ScopedHDCBase : public NonCopyable {
public:
	ScopedHDCBase(HWND& hwnd) : _hwnd(hwnd) , _hdc(nullptr) {}
	virtual ~ScopedHDCBase() = default;

	operator const HDC& () const { return _hdc; }

	void textOut(int x, int y, const wchar_t* text) const { g_textOut(_hdc, x, y, text); }

	void drawText(RECT* pos, const wchar_t* text, u32 fDT = kDefaultDT) const { g_drawText(_hdc, pos, text, fDT); }

	void rectangle(int l, int t, int r, int b) const { Rectangle(_hdc, l, t, r, b); }
	void rectangle(const RECT& r) const { rectangle(r.left, r.top, r.right, r.bottom); }

protected:
	HWND&	_hwnd;
	HDC		_hdc;
};

class ScopedPaint : public ScopedHDCBase {
	using Base = ScopedHDCBase;
public:
	ScopedPaint(HWND& hwnd, PAINTSTRUCT& ps)
		: Base(hwnd)
		, _ps(ps)
	{
		_hdc = BeginPaint(hwnd, &ps);
	}

	~ScopedPaint() { EndPaint(_hwnd, &_ps); }

	BOOL fErase()	const { return _ps.fErase; }
	RECT rcPaint()  const { return _ps.rcPaint; }

private:
	PAINTSTRUCT& _ps;
};

class ScopedHDC : public ScopedHDCBase {
	using Base = ScopedHDCBase;
public:
	ScopedHDC(HWND& hwnd) : Base(hwnd) { _hdc = GetDC(hwnd); }
	~ScopedHDC() { ReleaseDC(_hwnd, _hdc); }
};

}

#endif // SGE_OS_WINDOWS