#include "PW5_NativeUIWindow_Win32.h"

#if SGE_OS_WINDOWS

namespace sge {

void PW5_NativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	HMODULE hInstance = GetModuleHandle(nullptr);

	constexpr const wchar_t* clsName = L"PW5_NativeUIWindow_Win32";

	WNDCLASSEX wc = g_createWndClass(hInstance, clsName, s_wndProc);
	g_registerWndClass(wc);

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

void PW5_NativeUIWindow_Win32::onSetWindowTitle(StrView title) {
	if (!_hwnd) return;
	TempStringW tmp = UtfUtil::toStringW(title);
	SetWindowText(_hwnd, tmp.c_str());
}

void PW5_NativeUIWindow_Win32::onDraw() {
	ScopedPaintStruct ps(_hwnd);
	onPaint(ps);
}

LRESULT CALLBACK PW5_NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// CreateWindow(Ex) -> WM_CREATE -> ShowWindow -> WM_SHOWWINDOW -> WM_SIZE -> UpdateWindow -> WM_PAINT -> ... -> WM_CLOSE -> WM_DESTROY
	switch (msg)
	{
		case WM_CREATE: {
			auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisObj));
		} break;
		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd)) {
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
				thisObj->_hwnd = nullptr;
				sge_delete(thisObj);
			}
		} break;
		case WM_SHOWWINDOW: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onOpen();
			}
		} break;
		case WM_SIZE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				RECT clientRect;
				::GetClientRect(hwnd, &clientRect);
				Rect2f newClientRect = Win32Util::toRect2f(clientRect);
				thisObj->onClientRectChanged(newClientRect);
				return 0; // capture
			}
		} break;
		case WM_CLOSE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onCloseButton();
				return 0; // capture
			}
		} break;
		case WM_PAINT: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onDraw();
				return 0; // capture
			}
		} break;
		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				return thisObj->_handleNativeEvent(hwnd, msg, wParam, lParam);
			}
		} break;
	} // switch
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT PW5_NativeUIWindow_Win32::_handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

} // namespace

#endif // SGE_OS_WINDOWS