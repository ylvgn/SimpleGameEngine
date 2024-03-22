#if SGE_OS_WINDOWS

#include "PW5_MyHelloWinDestroy.h"

namespace sge {

void PW5_MyHelloWinDestroy::onCreate(CreateDesc& desc) {
	const wchar_t* clsName  = TEXT("PW5_MyHelloWinDestroy");
    WNDCLASS wndclass 		= {};
	auto hInstance			= GetModuleHandle(nullptr);
    wndclass.style			= CS_HREDRAW | CS_VREDRAW;	
    wndclass.cbClsExtra		= 0;
    wndclass.cbWndExtra		= 0;
    wndclass.hIcon			= LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor		= LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground	= 0;
    wndclass.lpszMenuName	= NULL;
    wndclass.hInstance		= hInstance;
    wndclass.lpfnWndProc	= s_wndProc;
    wndclass.lpszClassName	= clsName;

	RegisterClass(&wndclass);

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	_hwnd = CreateWindow(clsName, TEXT ("PW5_MyHelloWinDestroy"), dwStyle,
						 CW_USEDEFAULT, CW_USEDEFAULT,
						 CW_USEDEFAULT, CW_USEDEFAULT,
						 NULL, NULL, hInstance, this);

    ::ShowWindow (_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow (_hwnd);
}

void PW5_MyHelloWinDestroy::onCloseButton() {
	::DestroyWindow(_hwnd); // <------ not work when PW5_MyHelloWinDestroy is not new from heap
}

LRESULT CALLBACK PW5_MyHelloWinDestroy::s_wndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisObj));
		}break;

		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd)) {
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
				thisObj->_hwnd = nullptr;
				sge_delete(thisObj);
			}
		}break;

		case WM_CLOSE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onCloseButton();
				return 0;
			}
		}; break;
	}
    return DefWindowProc (hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS