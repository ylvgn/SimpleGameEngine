#if SGE_OS_WINDOWS

#include "PW5_MySysMets2.h"

namespace sge {

WNDPROC PW5_MySysMets2::s_defaultWndProc;

void PW5_MySysMets2::onCreate(CreateDesc& desc) {
	_dm = MySysmetricsDM::s_getMark();
	_scrollPosV = 0;

	desc.ownDC = true;
	Base::onCreate(desc);
	setWindowTitle("PW5_MySysMets2");
	s_defaultWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_wndProc)));
	
	const auto& sysmetrics = _dm->data();
	auto NUMLINES = static_cast<int>(sysmetrics.size());

	{
		ScopedGetDC hdc(_hwnd);
		auto tm = GDI::createTextMetrics(hdc);
		_cxChar = tm.aveCharWidth;
		_cxCaps = tm.aveCharWidthUpperCase;
		_cyChar = tm.aveCharHeight;
	}

	int clientRectH = static_cast<int>(_clientRect.h);
	int contentMaxH = static_cast<int>(_cyChar * NUMLINES);
	_viewportH = Math::max(0, contentMaxH - clientRectH);

	::SetScrollRange(_hwnd, SB_VERT, 0, _viewportH, false);
	::SetScrollPos(_hwnd, SB_VERT, _scrollPosV, true);
	::UpdateWindow(_hwnd);
}

void PW5_MySysMets2::onDraw() {
	ScopedGetDC hdc(_hwnd); // Even if you use CS_OWNDC, you should still release the device context handle before exiting the window procedure
	hdc.clearBg();

	const auto& sysmetrics	= _dm->data();
	auto NUMLINES			= static_cast<int>(sysmetrics.size());

	for (int i = 0; i < NUMLINES; i++) {
		int x = 0;
		int y = _cyChar * i - _scrollPosV;

		hdc.Fmt_textOut(x, y, "{:03d} {}", i, sysmetrics[i].name);

		x += 24 * _cxCaps;
		hdc.textOut(x, y, sysmetrics[i].mark);
		hdc.setTextAlign(PW5_TextAlignmentOption::Right | PW5_TextAlignmentOption::Top);

		x += 40 * _cxChar;
		hdc.Fmt_textOut(x, y, "{:5d}", ::GetSystemMetrics(sysmetrics[i].id));
		hdc.setTextAlign(PW5_TextAlignmentOption::Left | PW5_TextAlignmentOption::Top);
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
	::InvalidateRect(_hwnd, nullptr, true); // call WM_PAINT by windows message queue
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