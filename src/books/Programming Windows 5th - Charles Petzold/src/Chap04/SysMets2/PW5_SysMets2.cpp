#include "PW5_SysMets2.h"

#if SGE_OS_WINDOWS

namespace sge {

void PW5_SysMets2::onCreate(CreateDesc& desc) {
	const wchar_t* clsName = L"PW5_SysMets2";
	auto hInstance = ::GetModuleHandle(nullptr);

	auto wc = PW5_Win32Util::createWndClass(hInstance, clsName, s_wndProc);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)); // SetScrollPos will redraw using this brush to erase bg
	PW5_Win32Util::registerWndClass(wc);

	auto dwStyle = WS_OVERLAPPEDWINDOW | WS_VSCROLL;

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

LRESULT CALLBACK PW5_SysMets2::s_wndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int s_cxChar, s_cxCaps, s_cyChar, s_cyClient, s_iVscrollPos, s_textLineCount;

	auto* dm				= MySysmetricsDM::s_getMark();
	const auto& sysmetrics	= dm->data();
	auto NUMLINES			= static_cast<int>(sysmetrics.size());

	switch (message) {

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		case WM_CREATE: {
			TextMetrics tm(hwnd);
			s_cxChar = tm.aveCharWidth;
			s_cxCaps = tm.aveCharWidthUpperCase;
			s_cyChar = tm.aveCharHeight;

			// SetScrollRange:
				// When the thumb is at the top(or left) of the scroll bar, the position of the
				// thumb is the minimum value of the range.At the bottom(or right) of the scroll bar, the thumb position is
				// the maximum value of the range.
				// By default, the range of a scroll bar is 0 (top or left) through 100 (bottom or right)
			//  and you set bRedraw to TRUE if you want Windows to redraw the scroll bar based on the new range.
				//	(If you will be calling other functions that affect the appearance of the scroll bar after you call SetScrollRange
				// 	you'll probably want to set bRedraw to FALSE to avoid excessive redrawing.)
			::SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
			::SetScrollPos(hwnd, SB_VERT, s_iVscrollPos, TRUE);
		} break;

		case WM_LBUTTONDOWN: {
			// obtain the current range and position of a scroll bar
			int minRange, maxRange;
			::GetScrollRange(hwnd, SB_VERT, &minRange, &maxRange);

			int scrollVPos = ::GetScrollPos(hwnd, SB_VERT);
			SGE_DUMP_VAR(minRange, maxRange, scrollVPos);
		} break;

		case WM_SIZE: {
			// The low-order word of lParam specifies the new width of the client area
			// The high-order word of lParam specifies the new height of the client area
			s_cyClient = HIWORD(lParam);
			s_textLineCount = s_cyClient / s_cyChar;
		} break;

		case WM_VSCROLL: {
			// The low word of wParam is a number that indicates what the mouse is doing to the scroll bar.
			// This number is referred to as a "notification code."
			int request = LOWORD(wParam);
			switch (request)
			{
				// 'SB' -> scroll bar
				case SB_LINEUP:			s_iVscrollPos -= 1; break;
				case SB_LINEDOWN:		s_iVscrollPos += 1; break;
				case SB_PAGEUP:			s_iVscrollPos -= s_textLineCount; break;
				case SB_PAGEDOWN:		s_iVscrollPos += s_textLineCount; break;
				/*
					The HIWORD specifies the current position of the scroll box if the LOWORD is SB_THUMBPOSITION or SB_THUMBTRACK;
					otherwise, this word is not used.
				*/
				//case SB_THUMBTRACK:
				case SB_THUMBPOSITION:
					s_iVscrollPos = HIWORD(wParam); break; // The user has dragged the scroll box (thumb) and released the mouse button
				//case SB_ENDSCROLL: break;
				/*
					notification codes of SB_TOP, SB_BOTTOM, SB_LEFT, and SB_RIGHT, indicating that the scroll bar has been moved to its minimum or maximum position.
					However, you will never receive these notification codes for a scroll bar created as part of your application window
				*/
				default: break;
			}

			s_iVscrollPos = Math::max(0, Math::min(s_iVscrollPos, NUMLINES - 1));

			if (s_iVscrollPos != ::GetScrollPos(hwnd, SB_VERT))
			{
				/*
					You can generally ignore messages with the SB_ENDSCROLL notification code.
					Windows will not change the position of the scroll bar thumb. Your application does that by calling SetScrollPos.
				*/
				::SetScrollPos(hwnd, SB_VERT, s_iVscrollPos, TRUE);
				::InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		} break;

		case WM_PAINT: {
			// A user who scrolls down wants to bring a lower part of the document into view;
			// however, the program actually moves the document up in relation to the display window

			// The Window documentationand the header file identifiers are based on the user's perspective:
				// scroll up means moving toward the beginning of the document;
				// scroll down means moving toward the end
			ScopedPaintStruct ps(hwnd);
			const static int s_szLabelMaxWidth = 22 * s_cxCaps;
			const static int s_szDescMaxWidth  = 40 * s_cxChar;

			for (int i = 0; i < NUMLINES; i++) {
				int x = 0;
				int y = s_cyChar * (i - s_iVscrollPos);

				ps.textOut(x, y, sysmetrics[i].name);

				x += s_szLabelMaxWidth;
				ps.textOut(x, y, sysmetrics[i].mark);

				ps.setTextAlign(TextAlignmentOption::Right | TextAlignmentOption::Top);
				x += s_szDescMaxWidth;
				ps.Fmt_textOut(x, y, "{:5d}", ::GetSystemMetrics(sysmetrics[i].id));

				ps.setTextAlign(TextAlignmentOption::Left | TextAlignmentOption::Top);
			}
			return 0;
		} break;
	} // end switch
	return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS