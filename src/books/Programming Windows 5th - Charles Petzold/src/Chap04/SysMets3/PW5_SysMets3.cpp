#include "PW5_SysMets3.h"

#if SGE_OS_WINDOWS

namespace sge {

void PW5_SysMets3::onCreate(CreateDesc& desc) {
	const wchar_t* clsName = L"PW5_SysMets3";
	auto hInstance = ::GetModuleHandle(nullptr);

	auto wc = PW5_Win32Util::createWndClass(hInstance, clsName, s_wndProc);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)); // SetScrollPos will redraw using this brush to erase bg
	PW5_Win32Util::registerWndClass(wc);

	auto dwStyle = WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL;

	_hwnd = ::CreateWindowEx(0, clsName, clsName, dwStyle,
							 static_cast<int>(desc.rect.x),
							 static_cast<int>(desc.rect.y),
							 static_cast<int>(desc.rect.w),
							 static_cast<int>(desc.rect.h),
							 nullptr, nullptr, hInstance, this);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

	::ShowWindow(_hwnd, SW_NORMAL);
	::UpdateWindow(_hwnd);
}

LRESULT CALLBACK PW5_SysMets3::s_wndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	/*
		typedef struct tagSCROLLINFO
		{
			UINT cbSize;	// set to sizeof (SCROLLINFO)
			UINT fMask;		// values to set or get
			int nMin;		// minimum range value
			int nMax;		// maximum range value
			UINT nPage;		// page size
			int nPos;		// current position
			int nTrackPos;	// current tracking position
		} SCROLLINFO, * PSCROLLINFO;
	*/

	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;

	auto* dm				= MySysmetricsDM::s_getRemarks();
	const auto& sysmetrics	= dm->data();
	auto NUMLINES			= static_cast<int>(sysmetrics.size());

	switch (message) {
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		case WM_CREATE: {

			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisObj));

			ScopedGetDC hdc(hwnd);
			MyTextMetrics tm; tm.create(hdc);
			cxChar = tm.aveCharWidth;
			cxCaps = tm.aveCharWidthUpperCase;
			cyChar = tm.aveCharHeight;

			// Save the width of the three columns
			iMaxWidth = 40 * cxChar + 22 * cxCaps;

			{ // create scroll bar
				using CreateDesc = ScrollInfo::CreateDesc;
				using Axis = ScrollInfo::CreateDesc::Axis;
				{
					CreateDesc desc(Axis::Horizontal);
					thisObj->_siH.reset(static_cast<ScrollInfo*>(thisObj->createScrollBar(desc).detach()));
				}
				{
					CreateDesc desc(Axis::Vertical);
					thisObj->_siV.reset(static_cast<ScrollInfo*>(thisObj->createScrollBar(desc).detach()));
				}
			}
			
			return 0;
		}

		case WM_SIZE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				cxClient = LOWORD(lParam);
				cyClient = HIWORD(lParam);

				// Set vertical scroll bar range and page size
				thisObj->_siV->setRange(0, NUMLINES - 1);
				thisObj->_siV->setPage(cyClient / cyChar);
				thisObj->_siV->refresh();

				// Set horizontal scroll bar range and page size
				thisObj->_siH->setRange(0, 2 + iMaxWidth / cxChar);
				thisObj->_siH->setPage(cxClient / cxChar);
				thisObj->_siH->refresh();

				return 0;
			}
		} break;
		
		case WM_VSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				// Save the position for comparison later on
				int oldPos;
				thisObj->_siV->getPos(oldPos);

				int newPos = oldPos;
				int request = LOWORD(wParam);
				switch (request)
				{
					/*
						As you'll note, the WINUSER.H header files
						includes notification codes of SB_TOP, SB_BOTTOM, SB_LEFT, and SB_RIGHT,
						indicating that the scroll bar has been moved to its minimum or maximum position.
						However, you will never receive these notification codes for a scroll bar created as part of your application window
					*/
					//case SB_TOP:		newPos = thisObj->_siV->rangeMin(); break; // no use (superfluous code)
					//case SB_BOTTOM:	newPos = thisObj->_siV->rangeMax(); break; // no use (superfluous code)
					case SB_LINEUP:		newPos -= 1;							break;
					case SB_LINEDOWN:	newPos += 1;							break;
					case SB_PAGEUP:		newPos -= thisObj->_siV->_win32_page();	break;
					case SB_PAGEDOWN:	newPos += thisObj->_siV->_win32_page();	break;
					case SB_THUMBTRACK: thisObj->_siV->getTrackPos(newPos);		break;
				}
				thisObj->_siV->setPos(newPos);

				// If the position has changed, scroll the window and update it
				int curPos;
				thisObj->_siV->getPos(curPos);
				if (curPos != oldPos) {
					int deltaY = cyChar * (oldPos - curPos);
					::ScrollWindow(hwnd, 0, deltaY, nullptr, nullptr);
					::UpdateWindow(hwnd);
				}
				return 0;
			}
		} break;

		case WM_HSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				int oldPos;
				// Save the position for comparison later on
				thisObj->_siH->getPos(oldPos);

				int newPos = oldPos;
				int request = LOWORD(wParam);
				switch (request)
				{
					//case SB_LEFT:			newPos = thisObj->_siH->rangeMin(); break; // no use (superfluous code)
					//case SB_RIGHT:		newPos = thisObj->_siH->rangeMax(); break; // no use (superfluous code)
					case SB_LINELEFT:		newPos -= 1;							break;
					case SB_LINERIGHT:		newPos += 1;							break;
					case SB_PAGELEFT:		newPos -= thisObj->_siH->_win32_page();	break;
					case SB_PAGERIGHT:		newPos += thisObj->_siH->_win32_page();	break;
					case SB_THUMBPOSITION:	thisObj->_siH->getTrackPos(newPos);		break;
				}
				thisObj->_siH->setPos(newPos);

				// If the position has changed, scroll the window and update it
				int curPos;
				thisObj->_siH->getPos(curPos);
				if (curPos != oldPos) {
					int deltaX = cxChar * (oldPos - curPos);
					::ScrollWindow(hwnd, deltaX, 0, nullptr, nullptr);
					::UpdateWindow(hwnd);
				}
				return 0;
			}
		} break;

		case WM_PAINT: {
			if (auto* thisObj = s_getThis(hwnd)) {
				// Get vertical scroll bar position
				int iVertPos;
				thisObj->_siV->getPos(iVertPos);

				// Get horizontal scroll bar position
				int iHorzPos;
				thisObj->_siH->getPos(iHorzPos);

				// Find painting limits
				ScopedPaintStruct ps(hwnd);

				int iPaintBeg = Math::max(0, iVertPos + static_cast<int>(ps.rcPaint().top / cyChar));
				int iPaintEnd = Math::min(NUMLINES - 1, iVertPos + static_cast<int>(ps.rcPaint().bottom / cyChar));

				for (int i = iPaintBeg; i <= iPaintEnd; i++) {
					int x = cxChar * (1 - iHorzPos); // or 0
					int y = cyChar * (i - iVertPos);

					ps.textOut(x, y, sysmetrics[i].name);

					x += 22 * cxCaps;
					ps.textOut(x, y, sysmetrics[i].remarks);

					ps.setTextAlign(PW5_TextAlignmentOption::Right | PW5_TextAlignmentOption::Top);
					x += 40 * cxChar;
					ps.Fmt_textOut(x, y, "{:5d}", ::GetSystemMetrics(sysmetrics[i].id));

					ps.setTextAlign(PW5_TextAlignmentOption::Left | PW5_TextAlignmentOption::Top); // reset text align to left-top(default)
				}
				return 0;
			}
		} break;
	} // end switch

	return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS
