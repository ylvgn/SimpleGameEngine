#include "PW5_MySysMets2.h"

#if SGE_OS_WINDOWS

namespace sge {

WNDPROC PW5_MySysMets2::s_defaultWndProc;

void PW5_MySysMets2::onPostCreate() {
	s_defaultWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_wndProc)));
	::UpdateWindow(_hwnd);
}

void PW5_MySysMets2::onOpen() {
	// WM_SHOWWINDOW

	_scrollPosV = 0;

	const auto& NUMLINES = g_sysmetricsCount;

	{
		ScopedHDC hdc(_hwnd);
		auto tm = hdc.createTextMetrics();
		_cxChar = tm->aveCharWidth();
		_cxCaps = tm->aveUpperCaseCharWidth();
		_cyChar = tm->aveCharHeight();
	}
	
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	int clientRectH = static_cast<int>(rc.bottom);
	int contentMaxH = static_cast<int>(_cyChar * NUMLINES);
	_viewportH = Math::max(0, contentMaxH - clientRectH);

	::SetScrollRange(_hwnd, SB_VERT, 0, _viewportH, false);
	::SetScrollPos(_hwnd, SB_VERT, _scrollPosV, true);
}

void PW5_MySysMets2::onPaint(ScopedPaintStruct& ps) {
	// WM_PAINT

	int NUMLINES = static_cast<int>(g_sysmetricsCount);
	const auto& sysmetrics = g_sysmetrics;

	// clear bg
	auto brush = static_cast<HBRUSH>(GetStockBrush(WHITE_BRUSH));
	::SelectObject(ps, brush);
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	::FillRect(ps, &rc, brush);

	for (int i = 0; i < NUMLINES; i++) {
		int x = 0;
		int y = _cyChar * i - _scrollPosV;

		StrViewW s(sysmetrics[i].szLabel);
		ps.textOutf(x, y, "{:003} {}", i, s);

		x += 24 * _cxCaps;
		ps.textOut(x, y, sysmetrics[i].szDesc);
		::SetTextAlign(ps, TA_RIGHT | TA_TOP);

		x += 40 * _cxChar;
		ps.textOutf(x, y, "{:5}", ::GetSystemMetrics(sysmetrics[i].iIndex));
		::SetTextAlign(ps, TA_LEFT | TA_TOP);
	}
}

void PW5_MySysMets2::_onScrollV(int y) {
	y = Math::clamp(y, 0, _viewportH);
	::SetScrollPos(_hwnd, SB_VERT, y, true);

#if true
	// If you prefer to update the invalid area immediately, you can call UpdateWindow after you call InvalidateRect
	// UpdateWindow causes the window procedure to be called immediately with a WM_PAINT message if any part of the client area is invalid
	// (UpdateWindow will not call the window procedure if the entire client area is valid.)
	// In this case, the WM_PAINT message bypasses the message queue
	// UpdateWindow directs the window procedure(WM_PAINT) to paint it
	::ScrollWindow(_hwnd, 0, _scrollPosV - y, nullptr, nullptr);
	_scrollPosV = y;
	::UpdateWindow(_hwnd); // call WM_PAINT directly
#else
	// However, Windows treats WM_PAINT messages as low priority, so if a lot of other activity is occurring in the system,
	// it may be a while before your window procedure receives the WM_PAINT message.
	// Everyone has seen blank, white "holes" in Windows after a dialog box is removed and the program is still waiting to refresh its window.
	_scrollPosV = y;
	InvalidateRect(_hwnd, nullptr, true); // call WM_PAINT by windows message queue
#endif
}

LRESULT CALLBACK PW5_MySysMets2::s_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_VSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				int request = LOWORD(wParam);
				switch (request) {
					case SB_LINEUP:		thisObj->_onScrollV(thisObj->_scrollPosV - thisObj->_cyChar);	 break;
					case SB_LINEDOWN:	thisObj->_onScrollV(thisObj->_scrollPosV + thisObj->_cyChar);	 break;
					case SB_PAGEUP:		thisObj->_onScrollV(thisObj->_scrollPosV - thisObj->_viewportH); break;
					case SB_PAGEDOWN:	thisObj->_onScrollV(thisObj->_scrollPosV + thisObj->_viewportH); break;
					case SB_THUMBPOSITION:
					case SB_THUMBTRACK: thisObj->_onScrollV(HIWORD(wParam)); break;
				}
				return 0;
			}
		} break;
	} // end switch
	return s_defaultWndProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS