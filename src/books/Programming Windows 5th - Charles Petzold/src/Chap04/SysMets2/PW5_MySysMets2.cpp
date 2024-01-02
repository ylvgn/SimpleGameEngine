#include "PW5_MySysMets2.h"

#if SGE_OS_WINDOWS

namespace sge {

WNDPROC PW5_MySysMets2::s_defaultWndProc;

void PW5_MySysMets2::onPostCreate() {
	s_defaultWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_wndProc)));
	UpdateWindow(_hwnd);
}

void PW5_MySysMets2::onOpen() {
	// WM_SHOWWINDOW
	int NUMLINES = static_cast<int>(g_sysmetricsCount);

	_iVscrollPos = 0;
	{
		ScopedHDC hdc(_hwnd);
		auto tm = hdc.createTextMetrics();
		_cxChar = tm->aveCharWidth();
		_cxCaps = tm->aveUpperCaseCharWidth();
		_cyChar = tm->aveCharHeight();
	}
	
	RECT rc;
	GetClientRect(_hwnd, &rc);
	int clientRectHeight = static_cast<int>(rc.bottom);
	int contentMaxHeight = _cyChar * NUMLINES;
	int contentVRange = Math::max(0, contentMaxHeight - clientRectHeight);

	SetScrollRange(_hwnd, SB_VERT, 0, contentVRange, FALSE);
	SetScrollPos(_hwnd, SB_VERT, _iVscrollPos, TRUE);
}

void PW5_MySysMets2::onPaint(ScopedPaintStruct& ps) {
	// WM_PAINT
	int NUMLINES = static_cast<int>(g_sysmetricsCount);
	const auto& sysmetrics = g_sysmetrics;

	auto brush = static_cast<HBRUSH>(GetStockBrush(WHITE_BRUSH));
	SelectObject(ps, brush);
	RECT rc;
	GetClientRect(_hwnd, &rc);
	FillRect(ps, &rc, brush);

	for (int i = 0; i < NUMLINES; i++) {
		int x = 0;
		int y = _cyChar * i - _iVscrollPos;

		ps.textOut(x, y, sysmetrics[i].szLabel);

		x += 22 * _cxCaps;
		ps.textOut(x, y, sysmetrics[i].szDesc);
		SetTextAlign(ps, TA_RIGHT | TA_TOP);

		x += 40 * _cxChar;
		ps.textOutf(x, y, "{:5}", GetSystemMetrics(sysmetrics[i].iIndex));
		SetTextAlign(ps, TA_LEFT | TA_TOP);
	}
}

void PW5_MySysMets2::_onScrollV(int y) {
	_iVscrollPos = y;
	SetScrollPos(_hwnd, SB_VERT, y, TRUE);
	InvalidateRect(_hwnd, NULL, TRUE);
}

LRESULT CALLBACK PW5_MySysMets2::s_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_VSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				int request = LOWORD(wParam);
				thisObj->_cyClient = HIWORD(lParam);
				if (request == SB_THUMBPOSITION || request == SB_THUMBTRACK) {
					thisObj->_onScrollV(HIWORD(wParam));
					return 0;
				}
			}
		} break;
	} // switch
	return s_defaultWndProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS