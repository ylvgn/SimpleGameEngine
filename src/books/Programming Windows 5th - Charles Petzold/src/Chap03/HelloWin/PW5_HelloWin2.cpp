#include "PW5_HelloWin2.h"

#if SGE_OS_WINDOWS

namespace sge {

LRESULT CALLBACK PW5_HelloWin2::s_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
// WPARAM is UINT(32 bit) in Win32 and WORD(16 bit) in Win16
// LPARAM is LONG (4 bytes) 32 bit
	// LWORD (16 bit) L low-order,  [ 0~15] in LPARAM
	// HWORD (16 bit) H high-order, [16~31] in LPARAM

	switch (message)
	{
	case WM_CLOSE: {
		int ret = MessageBox(hwnd, L"Quit?", L"WM_CLOSE", MB_YESNO);
		if (ret == IDYES) {
			DestroyWindow(hwnd); // or DefWindowProc
		}
		return 0;
	} break;
	case WM_SIZE: {
		auto width  = static_cast<i16>(lParam & 0xffff);		// same as GET_X_LPARAM(lParam)
		auto height = static_cast<i16>(lParam >> 16 & 0xffff);	// same as GET_Y_LPARAM(lParam)
		SGE_LOG("WM_SIZE {}x{}\n", width, height);
	} break;
	case WM_LBUTTONDOWN: {
		auto x = GET_X_LPARAM(lParam); //  low-order word specifies the x-coordinate (16 bit)
		auto y = GET_Y_LPARAM(lParam); // high-order word specifies the y-coordinate (16 bit)

		HDC hdc = GetDC(hwnd);
		{
			Rectangle(hdc, x, y, x + 10, y + 10);

			const wchar_t* text = L"Hello Win2";
			TextOut(hdc, x, y, text, static_cast<int>(wcslen(text)) /* or lstrlenW(text)*/);

			POINT pt;
			MoveToEx(hdc, x, y, &pt);
			LineTo(hdc, x + 100, y);
		}
		ReleaseDC(hwnd, hdc);

		SGE_LOG("WM_LBUTTONDOWN ({}, {})\n", x, y);
		break;
	}
	case WM_PAINT: {
		RECT rect;
		PAINTSTRUCT ps;
		GetClientRect(hwnd, &rect);

		HDC hdc = BeginPaint(hwnd, &ps);
		{
			int sx = GetSystemMetrics(SM_CXSCREEN);
			int sy = GetSystemMetrics(SM_CYSCREEN);

			TCHAR text[128];
			_stprintf_s(text, 128, L"The display monitor resolution is %ix%i.", sx, sy);

			DrawText(hdc, text, -1 /*lstrlenW(text)*/, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		EndPaint(hwnd, &ps);
	} break;
    case WM_DESTROY:
        PostQuitMessage (0);
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS