#include "PW5_SysMets1.h"

namespace sge {

#if SGE_OS_WINDOWS

LRESULT CALLBACK PW5_SysMets1::s_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int  cxChar, cxCaps, cyChar;
	HDC         hdc;
	int         i;
	PAINTSTRUCT ps;
	TCHAR       szBuffer[10];
	TEXTMETRIC  tm;

	const auto& sysmetrics	= g_sysmetrics;
	const auto& NUMLINES	= g_sysmetricsCount;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);

		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hwnd, hdc);
		return 0;

	case WM_PAINT:
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
		hdc = BeginPaint(hwnd, &ps);

		for (i = 0; i < NUMLINES; i++) {
			TextOut(hdc, 0, cyChar * i,
				sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel));

			TextOut(hdc, 22 * cxCaps, cyChar * i,
				sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc));

			SetTextAlign(hdc, TA_RIGHT | TA_TOP);

			TextOut(hdc, 22 * cxCaps + 40 * cxChar, cyChar * i, szBuffer,
				wsprintf(szBuffer, TEXT("%5d"),
					GetSystemMetrics(sysmetrics[i].iIndex)));

			SetTextAlign(hdc, TA_LEFT | TA_TOP);
		}
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS