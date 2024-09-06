#if SGE_OS_WINDOWS

#include "OpenGLFalseContext_Win32.h"

namespace sge {
	
void OpenGLFalseContext_Win32::create() {
	destroy();

	static const wchar_t* className = L"OpenGLFalseContext_Win32";
	auto hInstance = GetModuleHandle(nullptr);

	::WNDCLASSEX wc;
	if (!GetClassInfoEx(hInstance, className, &wc)) {
		memset(&wc, 0, sizeof(wc));

		wc.cbSize			= sizeof(wc);
		wc.hInstance		= hInstance;
		wc.style			= CS_OWNDC;
		wc.lpfnWndProc		= DefWindowProc;
		wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground	= nullptr;
		wc.lpszMenuName		= nullptr;
		wc.lpszClassName	= className;

		if (!RegisterClassEx(&wc))
			throw SGE_ERROR("RegisterClassEx");
	}

	::DWORD dwExStyle = 0;
	::DWORD dwStyle = WS_POPUP;

	_hwnd = CreateWindowEx(dwExStyle, className, className, dwStyle,
						   0, 0, 0, 0,
						   nullptr, nullptr, hInstance, nullptr);

	if (!_hwnd)
		throw SGE_ERROR("CreateWindowEx");

	_dc = ::GetDC(_hwnd);
	if (!_dc)
		throw SGE_ERROR("GetDC");

	::PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize		= sizeof(pfd);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= 32;
	pfd.cDepthBits	= 32;
	pfd.iLayerType	= PFD_MAIN_PLANE;

	int nPixelFormat = ::ChoosePixelFormat(_dc, &pfd);
	if (!nPixelFormat)
		throw SGE_ERROR("ChoosePixelFormat");

	if (!::SetPixelFormat(_dc, nPixelFormat, &pfd))
		throw SGE_ERROR("SetPixelFormat");

	_rc = ::wglCreateContext(_dc);
	if (!_rc)
		throw SGE_ERROR("wglCreateContext");

	if (!::wglMakeCurrent(_dc, _rc))
		throw SGE_ERROR("wglMakeCurrent");
}

void OpenGLFalseContext_Win32::destroy() {
	if (_rc) {
		::wglDeleteContext(_rc);
		_rc = nullptr;
	}
	if (_dc) {
		SGE_ASSERT(_hwnd != nullptr);
		::ReleaseDC(_hwnd, _dc);
		_dc = nullptr;
	}
	if (_hwnd) {
		::DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}
}

}
#endif