#include "PW5_SysMets2.h"

#if SGE_OS_WINDOWS

namespace sge {

void PW5_SysMets2::onCreate(CreateDesc& desc) {
	auto hInstance = GetModuleHandle(nullptr);
	constexpr const static wchar_t* clsName = L"PW5_SysMets2";

	auto wc = g_createWndClass(hInstance, clsName, s_wndProc);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)); // SetScrollPos will redraw using this brush to erase bg
	g_registerWndClass(wc);

	DWORD dwStyle  = WS_OVERLAPPEDWINDOW | WS_VSCROLL;

	_hwnd = CreateWindowEx( 0, clsName, clsName, dwStyle,
							static_cast<int>(desc.rect.x),
							static_cast<int>(desc.rect.y),
							static_cast<int>(desc.rect.w),
							static_cast<int>(desc.rect.h),
							nullptr, nullptr, hInstance, nullptr);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

	ShowWindow(_hwnd, SW_NORMAL);
	UpdateWindow(_hwnd);
}

LRESULT CALLBACK PW5_SysMets2::s_wndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int cxChar, cxCaps, cyChar, cyClient, iVscrollPos;

	const auto& sysmetrics = g_sysmetrics;
	auto NUMLINES = static_cast<int>(g_sysmetricsCount);

	switch (message) {

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_CREATE: {
			TextMetrics tm(hwnd);
			cxChar = tm.aveCharWidth();
			cxCaps = tm.aveUpperCaseCharWidth();
			cyChar = tm.aveCharHeight();

			SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
			SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
		} break;

		case WM_SIZE: {
			// The low-order word of lParam specifies the new width of the client area
			// The high-order word of lParam specifies the new height of the client area
			cyClient = HIWORD(lParam);
		} break;

		case WM_VSCROLL: {
			int request = LOWORD(wParam);
			switch (request)
			{
				case SB_LINEUP:			iVscrollPos -= 1; break;
				case SB_LINEDOWN:		iVscrollPos += 1; break;
				case SB_PAGEUP:			iVscrollPos -= cyClient / cyChar; break;
				case SB_PAGEDOWN:		iVscrollPos += cyClient / cyChar; break;
				case SB_THUMBPOSITION:	iVscrollPos = HIWORD(wParam); break; // The user has dragged the scroll box (thumb) and released the mouse button
				default: break;
			}

			iVscrollPos = Math::max(0, Math::min(iVscrollPos, NUMLINES - 1));

			if (iVscrollPos != GetScrollPos(hwnd, SB_VERT))
			{
				SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		} break;

		case WM_PAINT: {
			ScopedPaintStruct ps(hwnd);
			for (int i = 0; i < NUMLINES; i++) {
				int x = 0;
				int y = cyChar * (i - iVscrollPos);

				ps.textOut(x, y, sysmetrics[i].szLabel);

				x += 22 * cxCaps;
				ps.textOut(x, y, sysmetrics[i].szDesc);
				SetTextAlign(ps, TA_RIGHT | TA_TOP);

				x += 40 * cxChar;
				ps.textOutf(x, y, "{:5}", GetSystemMetrics(sysmetrics[i].iIndex));
				SetTextAlign(ps, TA_LEFT | TA_TOP);
			}
			return 0;
		} break;

	} // switch
	return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS