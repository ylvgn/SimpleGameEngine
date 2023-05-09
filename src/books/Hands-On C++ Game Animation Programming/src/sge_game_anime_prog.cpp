#include <sge_core/native_ui/win32/Win32Util.h>

// https://registry.khronos.org/OpenGL/api/GL/wglext.h
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

GLuint gVertexArrayObject = 0;

namespace sge {

class MyGameAnimationProgrammingApp;

class MyGameAnimationProgrammingMainWin : public NonCopyable {
	using This = MyGameAnimationProgrammingMainWin;

	static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

public:
	struct MyGameAnimationProgrammingMainWin_CreteDesc {
		Rect2f rect{ 10, 10, 800, 600 };
	};

	using CreteDesc = MyGameAnimationProgrammingMainWin_CreteDesc;

	void create(CreteDesc& desc) {
		HINSTANCE hInstance = GetModuleHandle(NULL);

		const wchar_t* clsName = L"Win32 Game Window";

		WNDCLASSEX wndclass;
		wndclass				= {};
		wndclass.cbSize			= sizeof(WNDCLASSEX);
		wndclass.style			= CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc	= s_WndProc;
		wndclass.cbClsExtra		= 0;
		wndclass.cbWndExtra		= 0;
		wndclass.hInstance		= hInstance;
		wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
		wndclass.lpszMenuName	= 0;
		wndclass.lpszClassName	= clsName;
		RegisterClassEx(&wndclass);

		DWORD dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // WS_THICKFRAME to resize

		_hwnd = CreateWindowEx(0, clsName, clsName, dwStyle,
			static_cast<int>(desc.rect.x),
			static_cast<int>(desc.rect.y),
			static_cast<int>(desc.rect.w),
			static_cast<int>(desc.rect.h),
			nullptr, nullptr, hInstance, this);

		if (!_hwnd) {
			throw SGE_ERROR("cannot create native window");
		}

		ShowWindow(_hwnd, SW_SHOW);
		//UpdateWindow(_hwnd);
	}

	const HWND hwnd() { return _hwnd; }
	const Rect2f& clientRect() const { return _clientRect; }
	const HDC hdc() { return GetDC(_hwnd); }

protected:
	virtual void onClientRectChanged(const Rect2f& rc) { _clientRect = rc; }

	Rect2f	_clientRect{ 0,0,0,0 };

private:
	HWND _hwnd;

}; // MyGameAnimationProgrammingMainWin

class MyGameAnimationProgrammingApp : public NonCopyable {
public:

	struct MyGameAnimationProgrammingApp_CreteDesc {
	};
	using CreateDesc = MyGameAnimationProgrammingApp_CreteDesc;

	static MyGameAnimationProgrammingApp* instance() { return s_instance; }

	MyGameAnimationProgrammingApp() {
		SGE_ASSERT(s_instance == nullptr);
		s_instance = this;
	}

	virtual ~MyGameAnimationProgrammingApp() {
		SGE_ASSERT(s_instance == this);
		s_instance = nullptr;
	}

	void run(CreateDesc& ) {

		{ // create window
			MyGameAnimationProgrammingMainWin::CreteDesc desc;
			_mainWin.create(desc);
		}

		{ // create opengl render context

			HDC hdc = _mainWin.hdc();
			PIXELFORMATDESCRIPTOR pfd;
			pfd					= {}; // memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
			pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion		= 1;
			pfd.dwFlags			= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.iPixelType		= PFD_TYPE_RGBA;
			pfd.cColorBits		= 24;
			pfd.cDepthBits		= 32;
			pfd.cStencilBits	= 8;
			pfd.iLayerType		= PFD_MAIN_PLANE;
			int pixelFormat		= ChoosePixelFormat(hdc, &pfd);
			SetPixelFormat(hdc, pixelFormat, &pfd);

			// legacy render context
			HGLRC tempRC = wglCreateContext(hdc);
			wglMakeCurrent(hdc, tempRC);

			// legacy render context just for get function pointer of 'wglCreateContextAttribsARB'
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
			wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			const int attribList[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_FLAGS_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB,
				WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};

			// modern render context
			HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tempRC);
			wglMakeCurrent(hdc, hglrc);

			// use 'glad' to load all opengl core function
			if (!gladLoadGL()) {
				throw SGE_ERROR("Could not initialize GLAD\n");
			}

			SGE_LOG("OpenGL Version: {}.{} loaded", GLVersion.major, GLVersion.minor);
		}

		{ // vsynch: https://www.khronos.org/opengl/wiki/Swap_Interval
			PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
			bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

			_vsynch = 0;
			if (swapControlSupported) {
				PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
				PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

				if (wglSwapIntervalEXT(1)) {
					SGE_LOG("Enabled vsynch\n");
					_vsynch = wglGetSwapIntervalEXT();
				}
				else {
					SGE_LOG("Could not enable vsynch\n");
				}
			}
			else { // !swapControlSupported
				SGE_LOG("WGL_EXT_swap_control not supported\n");
			}

			glGenVertexArrays(1, &gVertexArrayObject);
			glBindVertexArray(gVertexArrayObject);
		}

		onInitialize();
		onRun();
	}

	void Shutdown() { onShutdown(); }

protected:
	void onRun() {
		HDC hdc = _mainWin.hdc();

		_lastTick = GetTickCount();
		while (GetMessage(&_win32_msg, NULL, 0, 0)) {
			TranslateMessage(&_win32_msg);
			DispatchMessage(&_win32_msg);

			DWORD thisTick = GetTickCount();
			float deltaTime = float(thisTick - _lastTick) * 0.001f;
			_lastTick = thisTick;
			onUpdate(deltaTime);

			const auto& clientRect = _mainWin.clientRect();
			float clientWidth = clientRect.size.x;
			float clientHeight = clientRect.size.y;
			float aspect = clientWidth / clientHeight;

			glViewport(0, 0, static_cast<GLsizei>(clientWidth), static_cast<GLsizei>(clientHeight));
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glPointSize(5.0f);
			glBindVertexArray(gVertexArrayObject);

			glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				
			onRender(aspect);
			SwapBuffers(hdc);
			if (_vsynch != 0) {
				glFinish();
			}
		} // End of game loop

		willquit();
	}

	void willquit() {
		SGE_LOG("Expected application to be null on exit\n");
	}

	virtual void onInitialize() { }
	virtual void onUpdate(float inDeltaTime) { }
	virtual void onRender(float inAspectRatio) { }
	virtual void onShutdown() {
		::PostQuitMessage(_exitCode);
	}

private:
	static MyGameAnimationProgrammingApp* s_instance;
	MyGameAnimationProgrammingMainWin _mainWin;

	int _exitCode = 0;
	MSG _win32_msg;
	DWORD _lastTick = 0;
	int _vsynch;

}; // MyGameAnimationProgrammingApp

MyGameAnimationProgrammingApp* MyGameAnimationProgrammingApp::s_instance = nullptr;

LRESULT CALLBACK MyGameAnimationProgrammingMainWin::s_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_CLOSE:
		MyGameAnimationProgrammingApp::instance()->Shutdown();
		break;
	case WM_DESTROY:
		if (gVertexArrayObject != 0) {
			HDC hdc = GetDC(hwnd);
			HGLRC hglrc = wglGetCurrentContext();

			// delete VAO
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &gVertexArrayObject);
			gVertexArrayObject = 0;

			// delete render context
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(hwnd, hdc);
		}
		else {
			SGE_LOG("Got multiple destroy messages\n");
		}
		break;
	case WM_SIZE: {
		if (auto* thisObj = s_getThis(hwnd)) {
			RECT clientRect;
			::GetClientRect(hwnd, &clientRect);
			Rect2f newClientRect = Win32Util::toRect2f(clientRect);
			thisObj->onClientRectChanged(newClientRect);
			return 0;
		}
	}break;
	case WM_PAINT:
	case WM_ERASEBKGND:
		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

} // namespace


int main(int argc, const char** argv) {
	using namespace sge;
	MyGameAnimationProgrammingApp app;
	MyGameAnimationProgrammingApp::CreateDesc desc;
	app.run(desc);

	return 0;
}