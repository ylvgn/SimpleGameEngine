#include "PW5_NativeUI.h"

namespace sge {

PW5_NativeUIApp* PW5_NativeUIApp::s_instance = nullptr;

PW5_NativeUIApp::PW5_NativeUIApp() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = this;
}

PW5_NativeUIApp::~PW5_NativeUIApp() {
	SGE_ASSERT(s_instance == this);
	s_instance = nullptr;
}

void PW5_NativeUIWindow::s_registerClass(HMODULE hInstance, const wchar_t* clsName, WNDPROC wndProc /*= nullptr*/) {
	auto wc = g_createWndClass(hInstance, clsName, wndProc);
	g_registerWndClass(wc);
}

void PW5_NativeUIWindow::s_createWindow(CreateDesc_Win32& desc) {
	Base::CreateDesc& baseDesc	= desc.baseDesc;
	WNDCLASSEX& wc				= desc.wc;
	auto hInstance				= wc.hInstance;
	const wchar_t* clsName		= wc.lpszClassName;

	if (!g_isRegisterWndClass(hInstance, clsName)) {
		s_registerClass(hInstance, clsName, wc.lpfnWndProc);
	}

	DWORD dwExStyle = 0;
	DWORD dwStyle   = WS_OVERLAPPEDWINDOW;

	if (baseDesc.centerToScreen) {
		int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		Vec2f screenSize = Vec2f(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
		baseDesc.rect.pos = (screenSize - baseDesc.rect.size) * 0.5f;
	}

	HWND hwnd = CreateWindowEx(	dwExStyle, clsName, clsName, dwStyle,
								static_cast<int>(baseDesc.rect.x),
								static_cast<int>(baseDesc.rect.y),
								static_cast<int>(baseDesc.rect.w),
								static_cast<int>(baseDesc.rect.h),
								nullptr, nullptr, hInstance, desc.cwlpParam);

	if (!hwnd) {
		throw SGE_ERROR("error create native window");
	}

	ShowWindow(hwnd, SW_NORMAL);
	UpdateWindow(hwnd);
}

}