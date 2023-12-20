#include "PW5_HelloWin3.h"

#if SGE_OS_WINDOWS

namespace sge {

void PW5_HelloWin3::onCreate(CreateDesc& desc) {
	HMODULE hInstance = GetModuleHandle(nullptr);

	const wchar_t* clsName = L"PW5_HelloWin3";

	WNDCLASSEX wc	 = g_createWndClass(clsName);
	wc.lpfnWndProc   = s_WndProc;
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));

    if (!RegisterClassEx(&wc)) {
		throw SGE_ERROR("error RegisterClassEx");
    }

	_hwnd = CreateWindowEx(	0, clsName, clsName, WS_OVERLAPPEDWINDOW,
							static_cast<int>(desc.rect.x),
							static_cast<int>(desc.rect.y),
							static_cast<int>(desc.rect.w),
							static_cast<int>(desc.rect.h),
							nullptr, nullptr, hInstance, this);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

	ShowWindow(_hwnd, SW_SHOWDEFAULT);
	UpdateWindow(_hwnd);
}

LRESULT CALLBACK PW5_HelloWin3::s_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_PAINT: {
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		
		{
			PAINTSTRUCT ps;
			ScopedPaint hdc(hwnd, ps);
			hdc.rectangle(clientRect);
			hdc.drawText(&clientRect, L"ClientRect");
		}
	} break;
    case WM_DESTROY:
        PostQuitMessage (0);
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS