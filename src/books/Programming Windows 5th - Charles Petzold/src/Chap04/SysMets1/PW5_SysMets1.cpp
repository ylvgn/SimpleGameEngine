#include "PW5_SysMets1.h"

namespace sge {

#if SGE_OS_WINDOWS

LRESULT CALLBACK PW5_SysMets1::s_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int  cxChar, cxCaps, cyChar;

	const auto& sysmetrics	= g_sysmetrics;
	const auto& NUMLINES	= g_sysmetricsCount;

	switch (message)
	{
		case WM_DESTROY: PostQuitMessage(0); break;
		case WM_CREATE: {
			MyTextMetric tm(hwnd);
			cxChar = tm.aveCharWidth();
			cxCaps = tm.aveUpperCaseCharWidth();
			cyChar = tm.aveCharHeight();
		} break;
		case WM_PAINT: {
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
				ps.textOut(x, y, sysmetrics[i].szLabel);

				// The longest uppercase identifier displayed in the first column is 20 characters,
				// so the second column must begin at least 20*cxCaps to the right of the beginning of the first column of text.
				// I use 22 to add a little extra space between the columns
				x += 22 * cxCaps;
				ps.textOut(x, y, sysmetrics[i].szDesc);
				SetTextAlign(ps.hdc(), TA_RIGHT | TA_TOP);

				x += 40 * cxChar;
				ps.textOutf(x, y, "{:5}", GetSystemMetrics(sysmetrics[i].iIndex)); //g_textOutf(ps.hdc(), x, y, L"%5d", GetSystemMetrics(sysmetrics[i].iIndex));
				SetTextAlign(ps.hdc(), TA_LEFT | TA_TOP);
			}
			return 0;
		} break;
	} // switch
	return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS