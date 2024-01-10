#pragma once

#if SGE_OS_WINDOWS

#include "PW5_Common.h"
#include <sge_core/native_ui/win32/Win32Util.h>

#include <windowsx.h>
#include <tchar.h>

namespace sge {

#if 0
#pragma mark ========= handy ============
#endif
inline
WNDCLASSEX g_createWndClass(HMODULE hInstance, const wchar_t* clsName, WNDPROC lpfnWndProc) {
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
	g_bzero(wc);
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

#if 0
#pragma mark ========= TextMetrics ============
#endif
class TextMetrics : public RefCountBase {
public:
	TextMetrics(HWND hwnd);
	TextMetrics(HDC hdc);
	TextMetrics(const TEXTMETRIC& tm);

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
	void	_set(const TEXTMETRIC& tm);
	void	_internal_init(HDC hdc);

	int		_height;
	int		_ascent;
	int		_descent;
	int		_internalLeading;
	int		_maxCharWidth;
	int		_externalLeading;
	int		_aveCharWidth;

	int		_aveCharHeight;
	int		_aveUpperCaseCharHeight;
	bool	_isFixedPitch : 1;
};

#if 0
#pragma mark ========= ScrollInfo ============
#endif
#define ScrollBarConstants_ENUM_LIST(E) \
	E(Horizontal,) \
	E(Vertical,) \
	E(Control,)  \
	E(Both,) \
//----
SGE_ENUM_CLASS(ScrollBarConstants, u8)

class ScrollInfo : public NonCopyable {
	using This = ScrollInfo;
public:

	ScrollInfo() : This(ScrollBarConstants::Vertical) { }

	ScrollInfo(ScrollBarConstants type) : _dirty(false) {
		reset();
		setType(type);
	}

	void reset();
	void reset(HWND hwnd, bool redraw = true);

	void setType(ScrollBarConstants type);

	SGE_NODISCARD ScrollInfo& setRange(int minRange, int maxRange) {
		if (_si.nMin != minRange || _si.nMax != maxRange) {
			_dirty = true;
			_si.fMask |= SIF_RANGE;

			_si.nMin = minRange;
			_si.nMax = maxRange;
		}
		return *this;
	}

	SGE_NODISCARD ScrollInfo& setPage(UINT newPage) {
		// set a proportional scroll bar thumb
		if (_si.nPage != newPage) {
			_dirty = true;
			_si.fMask |= SIF_PAGE;

			_si.nPage = newPage;
		}
		return *this;
	}

	SGE_NODISCARD ScrollInfo& setPos(int newPos) {
		if (_si.nPos != newPos) {
			_dirty = true;
			_si.fMask |= SIF_POS;

			_si.nPos = newPos;
		}
		return *this;
	}

	SGE_NODISCARD ScrollInfo& setTrackPos(HWND hwnd, int newTrackPos) {
		if (_si.nTrackPos != newTrackPos) {
			_dirty = true;
			_si.fMask |= SIF_TRACKPOS;

			_si.nTrackPos	= newTrackPos;
		}
		return *this;
	}

	void getRange(HWND hwnd, int& outMin, int& outMax) {
		_retrieve(hwnd, SIF_RANGE);
		outMin = _si.nMin;
		outMax = _si.nMax;
	}

	void getPage(HWND hwnd, UINT& out) {
		_retrieve(hwnd, SIF_PAGE);
		out = _si.nPage;
	}

	void getPos(HWND hwnd, int& out) {
		_retrieve(hwnd, SIF_POS);
		out = _si.nPos;
	}

	void getTrackPos(HWND hwnd, int& out) {
		// only while processing a WM_VSCROLL or WM_HSCROLL message
			// with a notification code of SB_THUMBTRACK or SB_THUMBPOSITION
		_retrieve(hwnd, SIF_TRACKPOS, false);
		out = _si.nTrackPos; // 32-bit thumb position
	}

	SCROLLINFO operator() (HWND hwnd) {
		_retrieve(hwnd);
		SCROLLINFO res = _si;
		res.fMask = SIF_ALL;
		return res;
	}

	int		rangeMin()	const { return _si.nMin;  }
	int		rangeMax()	const { return _si.nMax;  }
	UINT	page()		const { return _si.nPage; }
	int		pos()		const { return _si.nPos;  }
	//int	trackPos()	const { return _si.nTrackPos; } not provide such function, and use getTrackPos instead

private:
	void _retrieve(HWND hwnd, int flag = SIF_ALL, bool redraw = true) {
		reset(hwnd, redraw);
		ScopedValue<UINT> scoped(&_si.fMask, flag);
		GetScrollInfo(hwnd, _type, &_si);
	}

	bool		_dirty : 1;
	int			_type;

	SCROLLINFO	_si;
};

#if 0
#pragma mark ========= HDC ============
#endif
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
	void rectangle(const RECT& rc)   const	{ g_rectangle(_hdc, rc); }
	void rectangle(const Rect2i& rc) const	{ g_rectangle(_hdc, rc); }

	SPtr<TextMetrics> createTextMetrics() { return SPtr<TextMetrics>(new TextMetrics(_hdc)); }

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

}

#endif // SGE_OS_WINDOWS