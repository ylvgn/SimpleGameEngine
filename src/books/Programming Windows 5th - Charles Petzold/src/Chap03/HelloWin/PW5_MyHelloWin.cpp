#if SGE_OS_WINDOWS

#include "PW5_MyHelloWin.h"
#include <tchar.h>

namespace sge {

void PW5_MyHelloWin::onCreate(CreateDesc& desc) {
	const wchar_t* clsName = L"PW5_MyHelloWin";
	HMODULE hInstance = ::GetModuleHandle(nullptr);

	auto wc = PW5_Win32Util::createWndClass(hInstance, clsName, s_wndProc);
	PW5_Win32Util::registerWndClass(wc);

	auto dwStyle = WS_OVERLAPPEDWINDOW;

	_hwnd = ::CreateWindowEx(0, clsName, clsName, dwStyle,
							 static_cast<int>(desc.rect.x),
							 static_cast<int>(desc.rect.y),
							 static_cast<int>(desc.rect.w),
							 static_cast<int>(desc.rect.h),
							 nullptr, nullptr, hInstance, this);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

	::ShowWindow(_hwnd, SW_SHOW);
	::UpdateWindow(_hwnd);
}

LRESULT CALLBACK PW5_MyHelloWin::s_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
// WPARAM is UINT(32 bit) in Win32 and WORD(16 bit) in Win16
// LPARAM is LONG (4 bytes) 32 bit
	// LWORD (16 bit) L low-order,  [ 0~15] in LPARAM
	// HWORD (16 bit) H high-order, [16~31] in LPARAM

	static const int sTimerId = 9;
	static int sRed = 0;

	switch (message) {

		case WM_CREATE: {
			int elapse = 100; // us
			SetTimer(hwnd, sTimerId, elapse, nullptr);
		}break;

		case WM_TIMER: {
			SGE_LOG("wParam={}", wParam);
			if (wParam == sTimerId) { // better to set as enum
				InvalidateRect(hwnd, nullptr, true);
			}
		}break;

		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;

		case WM_CLOSE: {
			int ret = MessageBox(hwnd, L"Quit?", L"WM_CLOSE", MB_YESNO);
			if (ret == IDYES) {
				::DestroyWindow(hwnd); // or DefWindowProc
			}
			return 0;
		}

		case WM_SIZE: {
			auto width  = static_cast<i16>(lParam & 0xffff);		// same as GET_X_LPARAM(lParam)
			auto height = static_cast<i16>(lParam >> 16 & 0xffff);	// same as GET_Y_LPARAM(lParam)
			SGE_LOG("WM_SIZE {}x{}\n", width, height);
		} break;

		case WM_LBUTTONDOWN: {
			auto x = GET_X_LPARAM(lParam); //  low-order word specifies the x-coordinate (16 bit)
			auto y = GET_Y_LPARAM(lParam); // high-order word specifies the y-coordinate (16 bit)

			POINT pt;
			TempString s;
			Vec2i from, to;

			ScopedGetDC hdc(hwnd);
			hdc.rectangle(x, y, x + 10, y + 10);
			hdc.textOut(x, y, "MyHelloWin");

			from.x = x;
			from.y = y;
			to.x = x + 80;
			to.y = from.y;

			::MoveToEx(hdc, from.x, from.y, &pt);
			FmtTo(s, "p1:({},{})\n", pt.x, pt.y);
			GDI::lineTo(hdc, to.x, to.y);
			FmtTo(s, "line1:{} -> {}\n", from, to);

			from.x = x;
			from.y = y+15;
			to.x = x + 80;
			to.y = from.y;

			::MoveToEx(hdc, x, y+15, &pt);
			FmtTo(s, "p2:({},{})\n", pt.x, pt.y);
			GDI::lineTo(hdc, x + 80, y + 15);
			FmtTo(s, "line2:{} -> {}\n", from, to);

			SGE_LOG("WM_LBUTTONDOWN ({}, {})\n{}", x, y, s);
			break;
		}

		case WM_PAINT: {
			ScopedPaintStruct ps(hwnd);
			HDC hdc = ps.hdc();

			::RECT rc;
			::GetClientRect(hwnd, &rc);

			{
				sRed += 10;
				sRed = sRed % 256;
				
				ScopedCreateSolidBrush scoped(hdc, GDI::kbRed);
				int right = rc.left + 20;
				int bottom = rc.top + 20;
				ps.rectangle(rc.left, rc.top, right, bottom);

				char buf[64];
				SYSTEMTIME st;
				::GetSystemTime(&st);
				sprintf(buf, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
				ps.textOut(right, rc.top + 5, buf);
			}

			{
				int sx = ::GetSystemMetrics(SM_CXSCREEN);
				int sy = ::GetSystemMetrics(SM_CYSCREEN);
				TCHAR text[128];
				_stprintf_s(text, 128, L"The display monitor resolution is %ix%i.", sx, sy);
				::DrawText(ps, text, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		} break;
	} // end switch
    return DefWindowProc (hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS