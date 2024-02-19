#if SGE_OS_WINDOWS

#include "PW5_SysMets1.h"

namespace sge {

void PW5_SysMets1::onCreate(CreateDesc& desc) {
	const wchar_t* clsName = L"PW5_SysMets1";
	auto hInstance = ::GetModuleHandle(nullptr);

	auto wc = PW5_Win32Util::createWndClass(hInstance, clsName, s_wndProc);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)); // WM_SIZE->WM_PAINT->BeginPaint will redraw using this brush to erase bg
	PW5_Win32Util::registerWndClass(wc);

	auto dwStyle = WS_OVERLAPPEDWINDOW;

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

LRESULT CALLBACK PW5_SysMets1::s_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int cxChar, cxCaps, cyChar;

	switch (message) {
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		case WM_CREATE: {
			TextMetrics tm(hwnd);
			cxChar = tm.aveCharWidth;
			cxCaps = tm.aveCharWidthUpperCase;
			cyChar = tm.aveCharHeight;
		} break;

		case WM_PAINT: {
			auto* dm				= MySysmetricsDM::s_getMark();
			const auto& sysmetrics	= dm->data();
			auto NUMLINES			= static_cast<int>(sysmetrics.size());

			/*
				typedef struct tagPAINTSTRUCT {
					HDC hdc ;
					BOOL fErase ;			// FALSE (0), meaning that Windows has already erased the background of the invalid rectangle
					RECT rcPaint ;			// invalid rectangle
					BOOL fRestore ;			// Windows internal member
					BOOL fIncUpdate ;		// Windows internal member
					BYTE rgbReserved[32] ;  // Windows internal member
				} PAINTSTRUCT ;

				Windows fills in the fields of this structure when your program calls BeginPaint.
				Your program can use only the first three fields.
				The others are used internally by Windows
			*/
			ScopedPaintStruct ps(hwnd);
			for (int i = 0; i < NUMLINES; i++) {
				int x = 0;
				int y = cyChar * i;

				// The second argument to TextOut is 0 to begin the text at the left edge of the client area
				ps.textOut(x, y, sysmetrics[i].name);

				// The longest uppercase identifier displayed in the first column is 20 characters,
				// so the second column must begin at least 20*cxCaps to the right of the beginning of the first column of text.
				// use 22 to add a little extra space between the columns
				x += 22 * cxCaps;
				ps.textOut(x, y, sysmetrics[i].mark);
				ps.setTextAlign(PW5_TextAlignmentOption::Right | PW5_TextAlignmentOption::Top); // ::SetTextAlign(ps, TA_RIGHT | TA_TOP);

				x += 40 * cxChar;
				ps.Fmt_textOut(x, y, "{:5d}", ::GetSystemMetrics(sysmetrics[i].id));	// g_textOutf(ps.hdc(), x, y, L"%5d", GetSystemMetrics(sysmetrics[i].iIndex));
				ps.setTextAlign(PW5_TextAlignmentOption::Left | PW5_TextAlignmentOption::Top);	// reset text align to left-top(default)
			}
			return 0;
		} break;
	} // switch
	return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS