#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

void ProgWin5WindowBase::onCreate(CreateDesc& desc) {
	const wchar_t* clsName = L"ProgWin5WindowBase";

	WNDCLASSEX wc;
	g_bzero(wc);
	HMODULE hInstance	= GetModuleHandle(nullptr);
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

	if (desc.centerToScreen) {
		int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		Vec2f screenSize = Vec2f(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
		desc.rect.pos = (screenSize - desc.rect.size) * 0.5f;
	}

	_hwnd = CreateWindowEx(	dwExStyle, clsName, clsName, dwStyle,
							static_cast<int>(desc.rect.x),
							static_cast<int>(desc.rect.y),
							static_cast<int>(desc.rect.w),
							static_cast<int>(desc.rect.h),
							nullptr, nullptr, hInstance, this);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

    ShowWindow (_hwnd, SW_SHOWDEFAULT);
    UpdateWindow (_hwnd);
}

void ProgWin5WindowBase::onSetWindowTitle(StrView title) {
	if (!_hwnd) return;
	TempStringW tmp = UtfUtil::toStringW(title);
	SetWindowText(_hwnd, tmp.c_str());
}

LRESULT CALLBACK ProgWin5WindowBase::s_WndProcDefault(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE: {
		auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		auto* thisObj = static_cast<This*>(cs->lpCreateParams);
		thisObj->_hwnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisObj));
		return 0;
	} break;

	case WM_SIZE: {
		// CreateWindowEx -> WM_CREATE -> ShowWindow -> WM_SIZE -> UpdateWindow, ...
		// resize by user drag this window instance
		if (auto* thisObj = s_getThis(hwnd)) {
			RECT clientRect;
			::GetClientRect(hwnd, &clientRect);
			Rect2f newClientRect = Win32Util::toRect2f(clientRect);
			thisObj->onClientRectChanged(newClientRect);
			return 0;
		}
	}break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

} // namespace

#endif // SGE_OS_WINDOWS