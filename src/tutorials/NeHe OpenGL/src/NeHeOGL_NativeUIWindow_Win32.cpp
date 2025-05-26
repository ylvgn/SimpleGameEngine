#if SGE_OS_WINDOWS

#include "NeHeOGL_NativeUIWindow_Win32.h"

namespace sge {

void NeHeOGL_NativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	_camera.setPos(0, 10, 10);
	_camera.setAim(0, 0, 0);

	initGL();
}

void NeHeOGL_NativeUIWindow_Win32::initGL() {
	_dc = ::GetDC(_hwnd);
	if (!_dc)
		throw SGE_ERROR("initGL GetDC");

	::PIXELFORMATDESCRIPTOR pfd;		// pfd Tells Windows How We Want Things To Be
	g_bzero(pfd);
	pfd.nSize = sizeof(pfd);			// Size Of This Pixel Format Descriptor
	pfd.nVersion = 1;					// Version Number

	pfd.dwFlags = PFD_DOUBLEBUFFER		// Must Support Double Buffering
				| PFD_DRAW_TO_WINDOW	// Format Must Support Window
				| PFD_SUPPORT_OPENGL;	// Format Must Support OpenGL

	pfd.iPixelType = PFD_TYPE_RGBA;		// Request An RGBA Format
	pfd.cColorBits = 32;				// 32 bits Color, Number Of Bits 8/16/24/32
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
	pfd.cDepthBits = 32;				// 32 bits Z-Buffer(Depth Buffer)
	pfd.cStencilBits = 8;				// 8 bits Stencil Buffer
	// No Auxiliary Buffer
		// BYTE  cAuxBuffers;
	pfd.iLayerType = PFD_MAIN_PLANE;	// Main Drawing Layer
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

	onInitedGL();
	OGLUtil::throwIfError();
}

void NeHeOGL_NativeUIWindow_Win32::makeCurrent() {
	if (_rc) {
		if (!::wglMakeCurrent(_dc, _rc))
			throw SGE_ERROR("makeCurrent");
	}
}

void NeHeOGL_NativeUIWindow_Win32::swapBuffers() {
	if (_dc)
		::SwapBuffers(_dc);
}

void NeHeOGL_NativeUIWindow_Win32::clearGL() {
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

void NeHeOGL_NativeUIWindow_Win32::_destroy() {
	clearGL();
}

void NeHeOGL_NativeUIWindow_Win32::onCloseButton() {
	_destroy();
	NativeUIApp::current()->quit(0);
}

void NeHeOGL_NativeUIWindow_Win32::onUIMouseEvent(UIMouseEvent& ev) {
	using Button	= UIMouseEvent::Button;
	using Type		= UIMouseEvent::Type;
	using Modifier	= UIEventModifier;

	if (ev.isDragging()) {
		switch (ev.pressedButtons) {
			case Button::Left: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					auto d = ev.deltaPos * -0.005f;
					_camera.orbit(d.x, d.y);
				}
			}break;
			case Button::Middle: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					auto d = ev.deltaPos * 0.05f;
					_camera.move(d.x, d.y, 0);
				}
			}break;
			case Button::Right: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					auto d = ev.deltaPos * 0.03f;
					_camera.dolly(d.y);
				}
				else {
					auto d = ev.deltaPos * -0.005f;
					_camera.orbit(d.x, d.y);
				}
			}break;
		}
	}
	if (ev.isScroll()) {
		auto d = ev.scroll * -0.005f;
		_camera.dolly(d.y);
	}
}

void NeHeOGL_NativeUIWindow_Win32::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	if (ev.isDown(KeyCode::Ctrl) && ev.isDown(KeyCode::W)) {
		onCloseButton();
	}
	if (ev.isDown(KeyCode::F)) {
		SGE_DUMP_VAR(_camera.pos(), _camera.aim());
	}
}

void NeHeOGL_NativeUIWindow_Win32::setFrameBufferSize(const Vec2f& newSize) {
	_camera.setViewport(_clientRect);
	_matrix_proj = _camera.projMatrix();
	_matrix_view = _camera.viewMatrix();

	_frameBufferSize = newSize;
	glViewport(0, 0, static_cast<int>(newSize.x), static_cast<int>(newSize.y));

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glPointSize(1);
	glLineWidth(1);
	glColor4f(1, 1, 1, 1);

	glClearColor(0.0f, 0.2f, 0.2f, 0);
	glClearDepth(1);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(_matrix_proj._columns->data);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(_matrix_view._columns->data);
}

}

#endif