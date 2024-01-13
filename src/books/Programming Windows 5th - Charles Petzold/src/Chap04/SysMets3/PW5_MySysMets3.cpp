#include "PW5_MySysMets3.h"

#if SGE_OS_WINDOWS

namespace sge {

WNDPROC PW5_MySysMets3::s_defaultWndProc;

void PW5_MySysMets3::onPostCreate() {
	s_defaultWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_wndProc)));
	::UpdateWindow(_hwnd);
}

void PW5_MySysMets3::onOpen() {
	// WM_SHOWWINDOW

	{
		ScopedHDC hdc(_hwnd);
		auto tm = hdc.createTextMetrics();
		_cxChar = tm->aveCharWidth();
		_cxCaps = tm->aveUpperCaseCharWidth();
		_cyChar = tm->aveCharHeight();
	}

	_siV.reset(ScrollInfo::Type::Vertical);
	_siH.reset(ScrollInfo::Type::Horizontal);
}

void PW5_MySysMets3::onClientRectChanged(const Rect2f& rc) {
	// WM_SIZE
	Base::onClientRectChanged(rc);

	int NUMLINES		 = static_cast<int>(g_sysmetricsCount);
	int contentMaxHeight = _cyChar * NUMLINES;
	int contentMaxWidth  = 24 * _cxCaps + 40 * _cxChar;

	_siV.setRange(0, contentMaxHeight).setPage(static_cast<UINT>(_clientRect.h));
	_siH.setRange(0, contentMaxWidth).setPage(static_cast<UINT>(_clientRect.w));
}

void PW5_MySysMets3::onPaint(ScopedPaintStruct& ps) {
	// WM_PAINT

	int NUMLINES = static_cast<int>(g_sysmetricsCount);
	const auto& sysmetrics = g_sysmetrics;

	auto brush = static_cast<HBRUSH>(GetStockBrush(WHITE_BRUSH));
	::SelectObject(ps, brush);
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	::FillRect(ps, &rc, brush);

	int offsetY;
	_siV.getPos(_hwnd, offsetY);

	int offsetX;
	_siH.getPos(_hwnd, offsetX);
	
	for (int i = 0; i < NUMLINES; ++i) {
		int x = 0 - offsetX;
		int y = _cyChar * i - offsetY;

		StrViewW label(sysmetrics[i].szLabel);
		ps.textOutf(x, y, "{:003} {}", i, label);

		x += 24 * _cxCaps;
		ps.textOut(x, y, sysmetrics[i].szDesc);

		ps.setTextAlign(TextAlignment::Right | TextAlignment::Top);
		x += 40 * _cxChar;
		ps.textOutf(x, y, "{:5}", GetSystemMetrics(sysmetrics[i].iIndex));

		ps.setTextAlign(TextAlignment::Left | TextAlignment::Top); // reset text align
	}
}

void PW5_MySysMets3::_onScrollH(int newX) {
	int oldX = _siH.pos();
	_siH.setPos(newX); // internal will clamp the scroll pos

	int curX;
	_siH.getPos(_hwnd, curX);
	if (curX != oldX) {
		int deltaX = oldX - curX;
		::ScrollWindow(_hwnd, deltaX, 0, nullptr, nullptr);
		::UpdateWindow(_hwnd);
	}
}

void PW5_MySysMets3::_onScrollHLine(bool isLeft) {
	int delta = _cxChar;
	if (isLeft) delta = -delta;

	_onScrollH(_siH.pos() + delta);
}

void PW5_MySysMets3::_onScrollHPage(bool isLeft) {
	int delta = _siH.page();
	if (isLeft) delta = -delta;

	_onScrollH(_siH.pos() + delta);
}

void PW5_MySysMets3::_onScrollH(bool isLeft, bool isPage) {
	isPage ? _onScrollHPage(isLeft) : _onScrollHLine(isLeft);
}

void PW5_MySysMets3::_onScrollVLine(bool isUp) {
	int delta = _cyChar;
	if (isUp) delta = -delta;

	_onScrollV(_siV.pos() + delta);
}

void PW5_MySysMets3::_onScrollVPage(bool isUp) {
	int delta = _siV.page();
	if (isUp) delta = -delta;

	_onScrollV(_siV.pos() + delta);
}

void PW5_MySysMets3::_onScrollV(bool isUp, bool isPage) {
	isPage ? _onScrollVPage(isUp) : _onScrollVLine(isUp);
}

void PW5_MySysMets3::_onScrollV(int newY) {
	int oldY = _siV.pos();
	_siV.setPos(newY); // internal will clamp the scroll pos

	int curY;
	_siV.getPos(_hwnd, curY);
	if (curY != oldY) {
		int deltaY = oldY - curY;
		::ScrollWindow(_hwnd, 0, deltaY, nullptr, nullptr);
		::UpdateWindow(_hwnd);
	}
}

LRESULT CALLBACK PW5_MySysMets3::s_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

		case WM_KEYUP: {
			switch (wParam) {
				case VK_ESCAPE: // Esc
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					return 0;
				case VK_PRIOR: // Page Up
					if (auto* thisObj = s_getThis(hwnd)) {
						thisObj->_onScrollV(thisObj->_siV.rangeMin());
						return 0;
					} break;
				case VK_NEXT: // Page Down
					if (auto* thisObj = s_getThis(hwnd)) {
						thisObj->_onScrollV(thisObj->_siV.rangeMax());
						return 0;
					} break;
			} // end switch
		} break; // WM_KEYUP

		case WM_KEYDOWN: {
			switch (wParam) {
				case VK_UP:
					if (auto* thisObj = s_getThis(hwnd)) {
						thisObj->_onScrollV(true, GetAsyncKeyState(VK_CONTROL));
						return 0;
					} break;
				case VK_DOWN:
					if (auto* thisObj = s_getThis(hwnd)) {
						thisObj->_onScrollV(false, GetAsyncKeyState(VK_CONTROL));
						return 0;
					} break;
				case VK_LEFT:
					if (auto* thisObj = s_getThis(hwnd)) {
						thisObj->_onScrollH(true, GetAsyncKeyState(VK_CONTROL));
						return 0;
					} break;
				case VK_RIGHT:
					if (auto* thisObj = s_getThis(hwnd)) {
						thisObj->_onScrollH(false, GetAsyncKeyState(VK_CONTROL));
						return 0;
					} break;
			} // end switch
		} break; // WM_KEYDOWN

		case WM_MOUSEWHEEL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				auto newY = thisObj->_siV.pos() - GET_WHEEL_DELTA_WPARAM(wParam) * 0.5f;
				thisObj->_onScrollV(static_cast<int>(newY));
				return 0;
			}
		} break;
		
		// if your code run on a slow machine, 
		// and perhaps think about using the SB_SLOWMACHINE argument to GetSystemMetrics for alternative processing for slow machines

		case WM_HSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				int request = LOWORD(wParam);
				switch (request) {
					case SB_LINELEFT:	thisObj->_onScrollHLine(true);		 break;
					case SB_LINERIGHT:	thisObj->_onScrollHLine(false);		 break;
					case SB_PAGELEFT:	thisObj->_onScrollHPage(true);		 break;
					case SB_PAGERIGHT:	thisObj->_onScrollHPage(false);		 break;
					case SB_THUMBTRACK: thisObj->_onScrollH(HIWORD(wParam)); break; // no need SB_THUMBPOSITION is ok
				}
				return 0;
			}
		} break;

		case WM_VSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				int request = LOWORD(wParam);
				switch (request) {
					case SB_LINEUP:		thisObj->_onScrollVLine(true);		 break;
					case SB_LINEDOWN:	thisObj->_onScrollVLine(false);		 break;
					case SB_PAGEUP:		thisObj->_onScrollVPage(true);		 break;
					case SB_PAGEDOWN:	thisObj->_onScrollVPage(false);		 break;
					case SB_THUMBTRACK: thisObj->_onScrollV(HIWORD(wParam)); break; // no need SB_THUMBPOSITION is ok
				}
				return 0;
			}
		} break;
	} // end switch message
	return s_defaultWndProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS