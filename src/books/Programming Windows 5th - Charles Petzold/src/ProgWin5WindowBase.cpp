#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

void ProgWin5WindowBase::onCreate(CreateDesc& desc) {
	const wchar_t* clsName = L"ProgWin5WindowBase";

	HMODULE hInstance	= GetModuleHandle(nullptr);

	WNDCLASSEX wc;
	g_bzero(wc);

	wc.cbSize			= sizeof(WNDCLASSEX);
    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= onGetWndProc();
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= nullptr;
    wc.lpszMenuName		= nullptr;
    wc.lpszClassName	= clsName;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
		throw SGE_ERROR("error RegisterClassEx");
    }

	DWORD dwExStyle = 0;
	DWORD dwStyle   = WS_OVERLAPPEDWINDOW;

	_hwnd = CreateWindowEx(	dwExStyle, clsName, clsName, dwStyle,
							CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
							nullptr, nullptr, hInstance, nullptr);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

    ShowWindow (_hwnd, SW_SHOWDEFAULT);
    UpdateWindow (_hwnd);
}

}

#endif