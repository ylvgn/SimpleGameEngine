#include "MyOpenGLNativeUIWindow_Win32.h"

#if SGE_OS_WINDOWS

namespace sge {

void MyOpenGLNativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);
	initGL();
}

void MyOpenGLNativeUIWindow_Win32::onInitGL() {
	_dc = ::GetDC(_hwnd);
	if (!_dc)
		throw SGE_ERROR("GetDC");

	::PIXELFORMATDESCRIPTOR pfd;
	pfd = { 0 };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;

	pfd.dwFlags = PFD_DOUBLEBUFFER
				| PFD_DRAW_TO_WINDOW
				| PFD_SUPPORT_OPENGL;

	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pf = ::ChoosePixelFormat(_dc, &pfd);
	if (!pf)
		throw SGE_ERROR("ChoosePixelFormat");

	if (!::SetPixelFormat(_dc, pf, &pfd))
		throw SGE_ERROR("SetPixelFormat");

	_rc = ::wglCreateContext(_dc);
	if (!_rc)
		throw SGE_ERROR("wglCreateContext");

	_makeCurrent();
}

void MyOpenGLNativeUIWindow_Win32::onDestroyGL() {
	if (_rc) {
		::wglMakeCurrent(nullptr, nullptr);
		::wglDeleteContext(_rc);
		_rc = nullptr;
	}
	if (_dc) {
		SGE_ASSERT(_hwnd != nullptr);
		::ReleaseDC(_hwnd, _dc);
		_dc = nullptr;
	}
}

void MyOpenGLNativeUIWindow_Win32::_makeCurrent() {
	if (_rc) {
		if (!::wglMakeCurrent(_dc, _rc))
			throw SGE_ERROR("_makeCurrent");
	}
}

void MyOpenGLNativeUIWindow_Win32::onSwapBuffers() {
	if (_dc)
		::SwapBuffers(_dc);
}

}

#endif