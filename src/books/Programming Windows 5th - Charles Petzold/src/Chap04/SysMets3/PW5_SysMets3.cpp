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
							 nullptr, nullptr, hInstance, nullptr);

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
	using ScrollInfo = PW5_SysMets3::ScrollInfo;

	static ScrollInfo siV;
	static ScrollInfo siH;

	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;

	auto* dm				= MySysmetricsDM::s_getMark();
	const auto& sysmetrics	= dm->data();
	auto NUMLINES			= static_cast<int>(sysmetrics.size());

	switch (message) {

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		case WM_CREATE: {
			TextMetrics tm(hwnd);
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
					siH.create(desc);
				}
				{
					CreateDesc desc(Axis::Vertical);
					siV.create(desc);
				}
			}
			
			return 0;
		}

		case WM_SIZE: {
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);

			// Set vertical scroll bar range and page size
			siV.setRange(0, NUMLINES - 1);
			siV.setPage(cyClient / cyChar);
			siV.reset(hwnd);

			// Set horizontal scroll bar range and page size
			siH.setRange(0, 2 + iMaxWidth / cxChar);
			siH.setPage(cxClient / cxChar);
			siH.reset(hwnd);

			return 0;
		}
		
		case WM_VSCROLL: {
			// Save the position for comparison later on
			int iOldPos;
			siV.getPos(hwnd, iOldPos);

			int newPos = siV.pos();

			int request = LOWORD(wParam);
			switch (request)
			{
				/*
					As you'll note, the WINUSER.H header files
					includes notification codes of SB_TOP, SB_BOTTOM, SB_LEFT, and SB_RIGHT,
					indicating that the scroll bar has been moved to its minimum or maximum position.
					However, you will never receive these notification codes for a scroll bar created as part of your application window
				*/
				//case SB_TOP:		newPos = siV.rangeMin(); break; // no use (superfluous code)
				//case SB_BOTTOM:	newPos = siV.rangeMax(); break; // no use (superfluous code)
				case SB_LINEUP:		newPos -= 1;					break;
				case SB_LINEDOWN:	newPos += 1;					break;
				case SB_PAGEUP:		newPos -= siV.page();			break;
				case SB_PAGEDOWN:	newPos += siV.page();			break;
				case SB_THUMBTRACK: siV.getTrackPos(hwnd, newPos);	break;
			}
			siV.setPos(newPos);

			// If the position has changed, scroll the window and update it
			int curPos;
			siV.getPos(hwnd, curPos);
			if (curPos != iOldPos) {
				int deltaY = cyChar * (iOldPos - curPos);
				::ScrollWindow(hwnd, 0, deltaY, nullptr, nullptr);
				::UpdateWindow(hwnd);
			}
			return 0;
		}

		case WM_HSCROLL: {
			int iOldPos;
			// Save the position for comparison later on
			siH.getPos(hwnd, iOldPos);

			int newPos = siH.pos();

			int request = LOWORD(wParam);
			switch (request)
			{
				//case SB_LEFT:			newPos = siH.rangeMin(); break; // no use (superfluous code)
				//case SB_RIGHT:		newPos = siH.rangeMax(); break; // no use (superfluous code)
				case SB_LINELEFT:		newPos -= 1;					break;
				case SB_LINERIGHT:		newPos += 1;					break;
				case SB_PAGELEFT:		newPos -= siH.page();			break;
				case SB_PAGERIGHT:		newPos += siH.page();			break;
				case SB_THUMBPOSITION:	siH.getTrackPos(hwnd, newPos);	break;
			}
			siH.setPos(newPos);

			// If the position has changed, scroll the window and update it
			int curPos;
			siH.getPos(hwnd, curPos);
			if (curPos != iOldPos) {
				int deltaX = cxChar * (iOldPos - curPos);
				::ScrollWindow(hwnd, deltaX, 0, nullptr, nullptr);
				::UpdateWindow(hwnd);
			}
			return 0;
		}

		case WM_PAINT: {
			// Get vertical scroll bar position
			int iVertPos;
			siV.getPos(hwnd, iVertPos);

			// Get horizontal scroll bar position
			int iHorzPos;
			siH.getPos(hwnd, iHorzPos);

			// Find painting limits
			ScopedPaintStruct ps(hwnd);

			int iPaintBeg = Math::max(0, iVertPos + static_cast<int>(ps.rcPaint().top / cyChar));
			int iPaintEnd = Math::min(NUMLINES - 1, iVertPos + static_cast<int>(ps.rcPaint().bottom / cyChar));

			for (int i = iPaintBeg; i <= iPaintEnd; i++) {
				int x = cxChar * (1 - iHorzPos); // or 0
				int y = cyChar * (i - iVertPos);

				ps.textOut(x, y, sysmetrics[i].name);

				x += 22 * cxCaps;
				ps.textOut(x, y, sysmetrics[i].mark);

				ps.setTextAlign(TextAlignmentOption::Right | TextAlignmentOption::Top);
				x += 40 * cxChar;
				ps.Fmt_textOut(x, y, "{:5d}", ::GetSystemMetrics(sysmetrics[i].id));

				ps.setTextAlign(TextAlignmentOption::Left | TextAlignmentOption::Top); // reset text align to left-top(default)
			}
			return 0;
		}
	} // end switch
	return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS
