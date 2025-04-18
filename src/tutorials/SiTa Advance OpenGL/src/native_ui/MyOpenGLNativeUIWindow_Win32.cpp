#include "MyOpenGLNativeUIWindow_Win32.h"

#if SGE_OS_WINDOWS

namespace sge {

#if 0
#pragma mark ================= MyOpenGLWindow_FalseContext ====================
#endif
class MyOpenGLWindow_FalseContext : public NonCopyable {
public:
	~MyOpenGLWindow_FalseContext() { destroy(); }

	void create() {
		destroy();

		static const wchar_t* className = L"MyOpenGLWindow_FalseContext";

		auto hInstance = GetModuleHandle(nullptr);

		//-- Check did we register window class yet
		::WNDCLASSEX wc;
		if (!GetClassInfoEx(hInstance, className, &wc)) {
			//-- register window class
			memset(&wc, 0, sizeof(wc));
			wc.cbSize			= sizeof(wc);
			wc.hInstance		= hInstance;
			wc.style			= CS_OWNDC; //!! <------- CS_OWNDC is required for OpenGL Window
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

		_hwnd = CreateWindowEx( dwExStyle, className, className, dwStyle,
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

	void destroy() {
		if (_rc) {
			//::wglMakeCurrent(nullptr, nullptr); //!! <------- MyOpenGLNativeUIWindow_Win32 will set new current, no need set nullptr
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

private:
	::HWND	_hwnd	= nullptr;
	::HDC	_dc		= nullptr;
	::HGLRC	_rc		= nullptr;
};


#if 0
#pragma mark ================= MyOpenGLNativeUIWindow_Win32 ====================
#endif
void MyOpenGLNativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	destroy();

	// create False context in order to init glew to have wglCreateContextAttribsARB()
	MyOpenGLWindow_FalseContext falseContext;
	falseContext.create();
	glewInit();

	desc.ownDC = true;
	Base::onCreate(desc);

	if (!_hwnd)
		throw SGE_ERROR("CreateWindow");

	_dc = ::GetDC(_hwnd);
	if (!_dc)
		throw SGE_ERROR("GetDC");

	// create context
	const int formatAttrs[] = { 
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,	GL_TRUE,
		WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,		32,
		WGL_DEPTH_BITS_ARB,		24,
		WGL_STENCIL_BITS_ARB,	8,
		0 // End of attributes list
	}; 

	int contextAttrs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,

	#if 0 // disable deprecated function, such as glBegin, glColor, glLoadMatrix
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	#else
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
	#endif

		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0 // End of attributes list
	};

	int format, numFormat;
	if (!::wglChoosePixelFormatARB(_dc, formatAttrs, nullptr, 1, &format, reinterpret_cast<UINT*>(&numFormat))) {
		throw SGE_ERROR("wglChoosePixelFormatARB");
	}

	::PIXELFORMATDESCRIPTOR pfd;
	if (!::SetPixelFormat(_dc, format, &pfd)) {
		throw SGE_ERROR("SetPixelFormat");
	}

	::HGLRC sharedContext = nullptr;
	_rc = ::wglCreateContextAttribsARB(_dc, sharedContext, contextAttrs);
	if (!_rc)
		throw SGE_ERROR("wglCreateContextAttribsARB");

	_makeCurrent();

	onInitGL();

	::ShowWindow(_hwnd, SW_SHOW);
}

void MyOpenGLNativeUIWindow_Win32::destroy() {
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

void MyOpenGLNativeUIWindow_Win32::onBeginRender() {
	setFrameBufferSize(_clientRect.size);
}

void MyOpenGLNativeUIWindow_Win32::onEndRender() {
	swapBuffers();
	drawNeeded();
}

}

#endif // SGE_OS_WINDOWS