#if SGE_OS_WINDOWS

#include "NeheOGL_NativeUIWindow_Win32.h"

namespace sge {

void NeheOGL_NativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);

	_initGL();
}

void NeheOGL_NativeUIWindow_Win32::_initGL() {
	_dc = ::GetDC(_hwnd);
	if (!_dc)
		throw SGE_ERROR("GetDC");

	PIXELFORMATDESCRIPTOR pfd;		// pfd Tells Windows How We Want Things To Be
	g_bzero(pfd);
	pfd.nSize = sizeof(pfd);		// Size Of This Pixel Format Descriptor
	pfd.nVersion = 1;				// Version Number
	pfd.dwFlags = PFD_DOUBLEBUFFER	// Must Support Double Buffering
		| PFD_DRAW_TO_WINDOW		// Format Must Support Window
		| PFD_SUPPORT_OPENGL;		// Format Must Support OpenGL

	pfd.iPixelType = PFD_TYPE_RGBA; // Request An RGBA Format
	pfd.cColorBits = 32;			// 32 bits Color, Number Of Bits 8/16/24/32
	// Color Bits Ignored
		// BYTE  cRedBits;
		// BYTE  cRedShift;
		// BYTE  cGreenBits;
		// BYTE  cGreenShift;
		// BYTE  cBlueBits;
		// BYTE  cBlueShift;
	// Alpha Bits Ignored
		// BYTE  cAlphaBits;
		// BYTE  cAlphaShift;
	// Accumulation Bits Ignored
		// BYTE  cAccumBits;
		// BYTE  cAccumRedBits;
		// BYTE  cAccumGreenBits;
		// BYTE  cAccumBlueBits;
		// BYTE  cAccumAlphaBits;
	pfd.cDepthBits = 32; // 32 bits Z-Buffer (Depth Buffer)
	pfd.cStencilBits = 8; // 8 bits Stencil Buffer
	// No Auxiliary Buffer
		// BYTE  cAuxBuffers;
	pfd.iLayerType = PFD_MAIN_PLANE; // Main Drawing Layer
	// bReserved 0, Layer Masks Ignored
		// BYTE  bReserved;
		// DWORD dwLayerMask;
		// DWORD dwVisibleMask;
		// DWORD dwDamageMask;

	int pf = ::ChoosePixelFormat(_dc, &pfd);
	if (pf == 0) // Did Windows Find A Matching Pixel Format?
		throw SGE_ERROR("ChoosePixelFormat");

	if (!::SetPixelFormat(_dc, pf, &pfd)) // Are We Able To Set The Pixel Format?
		throw SGE_ERROR("SetPixelFormat");

	_rc = ::wglCreateContext(_dc);
	if (!_rc) // Are We Able To Get A Rendering Context?
		throw SGE_ERROR("wglCreateContext");

	makeCurrent();
}

void NeheOGL_NativeUIWindow_Win32::destroy() {
	if (_rc) {
		::wglMakeCurrent(nullptr, nullptr);
		::wglDeleteContext(_rc);
		_rc = nullptr;
	}

	if (_dc) {
		::ReleaseDC(_hwnd, _dc);
		_dc = nullptr;
	}

#if 0 // maybe no need ???
	if (_hwnd) {
		::DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}
#endif
}

void NeheOGL_NativeUIWindow_Win32::makeCurrent() {
	if (_rc) {
		if (!::wglMakeCurrent(_dc, _rc))
			throw SGE_ERROR("makeCurrent");
	}
}

void NeheOGL_NativeUIWindow_Win32::swapBuffers() {
	if (_dc)
		::SwapBuffers(_dc);
}

void NeheOGL_NativeUIWindow_Win32::onCloseButton() {
	//destroy(); maybe no need ???
	NativeUIApp::current()->quit(0);
}

}

#endif