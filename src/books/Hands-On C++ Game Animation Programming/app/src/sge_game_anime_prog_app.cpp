#include <sge_game_anime_prog.h>
#include "SampleRequest.h"
#include "SampleContext.h"
#include "TimingInfo.h"

namespace sge {

// OpenGL: https://registry.khronos.org/OpenGL/api/GL/wglext.h
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

// VSynch
typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL		(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int			(WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

class GameAnimeProgMainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
	using SampleType = MySampleType;
public:
	static constexpr const int		kSampleCount		= enumInt(SampleType::_END);
	static constexpr const float	kMouseDecayFactor	= 0.3f;
	static constexpr const float	kMouseMaxDelta		= 10.f;

	void update(float dt) {
		_sampleRequest.dt = dt * _dtFactor;

		if (_vertexArrayObject == 0)
			return;

#if SGE_OS_WINDOWS
		_timingInfo.cpu->beginUpdate(_sampleRequest.dt);
#endif

		_cameraMove();

		if (_sampleContext)
			_sampleContext->update(_sampleRequest);

#if SGE_OS_WINDOWS
		_timingInfo.cpu->endUpdate(_sampleRequest.dt);
#endif
	}

	void render() {
		if (_vertexArrayObject == 0)
			return;

		_beginRender();

		if (_sampleContext) {
#if SGE_OS_WINDOWS
			_timingInfo.cpu->beginAppRender();
			_timingInfo.gpu->beginAppRender();
#endif
			_sampleContext->render(_sampleRequest);

#if SGE_OS_WINDOWS
			_timingInfo.gpu->endAppRender();
			_timingInfo.cpu->endAppRender();
#endif
		}

		_drawUI();

		_endRender();
	}

protected:
	virtual void onCreate(CreateDesc& desc) override  {
		Base::onCreate(desc);

		_hdc = ::GetDC(_hwnd);

		{ // create OpenGL render context
			::PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(pfd));
			pfd.nSize 			= sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion 		= 1;
			pfd.dwFlags 		= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.iPixelType 		= PFD_TYPE_RGBA;
			pfd.cColorBits 		= 32;
			pfd.cDepthBits 		= 32;
			pfd.cStencilBits 	= 8;
			pfd.iLayerType 		= PFD_MAIN_PLANE;

			int pixelFormat 	= ::ChoosePixelFormat(_hdc, &pfd);
			::SetPixelFormat(_hdc, pixelFormat, &pfd);

			// legacy render context
			::HGLRC tempRC = ::wglCreateContext(_hdc);
			::wglMakeCurrent(_hdc, tempRC);

			// legacy render context just for get function pointer of 'wglCreateContextAttribsARB'
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(::wglGetProcAddress("wglCreateContextAttribsARB"));
			const int attribList[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,

				WGL_CONTEXT_FLAGS_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0, // End of attributes list
			};

			// modern render context
			::HGLRC hglrc = wglCreateContextAttribsARB(_hdc, 0, attribList);

			::wglMakeCurrent(NULL, NULL);
			::wglDeleteContext(tempRC);
			::wglMakeCurrent(_hdc, hglrc);

			// use 'glad' to load all OpenGL core function
			if (!::gladLoadGL()) {
				throw SGE_ERROR("Could not initialize GLAD\n");
			}

			SGE_LOG("OpenGL Version: {}.{} loaded", GLVersion.major, GLVersion.minor);
		}

		{ // VSynch: https://www.khronos.org/opengl/wiki/Swap_Interval
			PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(wglGetProcAddress("wglGetExtensionsStringEXT"));
			bool isSwapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

			_vsynch = 0;
			if (isSwapControlSupported) {
				PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
				PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(wglGetProcAddress("wglGetSwapIntervalEXT"));

				if (wglSwapIntervalEXT(1)) {
					SGE_LOG("Enabled VSynch\n");
					_vsynch = wglGetSwapIntervalEXT();
				}
				else {
					SGE_LOG("Could not enable VSynch\n");
				}
			}
			else { // !swapControlSupported
				SGE_LOG("WGL_EXT_swap_control not supported\n");
			}
		}

		// Setup some OpenGL required state
		glGenVertexArrays(1, &_vertexArrayObject);
		glBindVertexArray(_vertexArrayObject);

#if SGE_OS_WINDOWS
		_timingInfo.create(_hwnd);
#endif

		_camera.setPos(0, 10, 10);
		_camera.setAim(0, 0, 0);

		NuklearUI::createContext(); // create Nuklear (kind of ImGui)

		_createSample();
	}

	virtual void onCloseButton() override {
		if (_vertexArrayObject != 0) {
			::HGLRC hglrc = ::wglGetCurrentContext();

			// delete VAO
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &_vertexArrayObject);
			_vertexArrayObject = 0;

			// delete Nuklear
			NuklearUI::destroyContext();

			// delete render context
			::wglMakeCurrent(NULL, NULL);
			::wglDeleteContext(hglrc);
		}

		::ReleaseDC(_hwnd, _hdc);
		NativeUIApp::current()->quit(0);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		using Button = UIMouseEventButton;

		if (NuklearUI::onUIMouseEvent(ev)) {
			return;
		}

		if (ev.isDragging()) {
			switch (ev.pressedButtons) {
				case Button::Left: {
					auto d = ev.deltaPos * 0.005f;
					_camera.pan(-d.x, d.y);
				}break;
				case Button::Middle: {
					auto d = ev.deltaPos * 0.05f;
					_camera.move(d.x, d.y, 0);
				}break;
				case Button::Right: {
					auto d = ev.deltaPos * 0.005f;
					_camera.orbit(d.x, d.y);
				}break;
			}
		}
		if (ev.isScroll()) {
			auto d = ev.scroll * -0.02f;
			_camera.dolly(d.y);
		}
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		using KeyCode = UIKeyboardEventKeyCode;

		NuklearUI::onUIKeyboardEvent(ev);

		if (ev.isDown(KeyCode::Escape)) {
			_exitSample();
		}
		if (ev.isDown(KeyCode::Tab)) {
			if (ev.isDown(KeyCode::Shift)) {
				_switchToLastSample();
			} else {
				_switchToNextSample();
			}
		}

		if (ev.isDown(KeyCode::Space)) {
			_bShowSettingWindow = !_bShowSettingWindow;
		}

		if (ev.isDown(KeyCode::Ctrl)) {
			static constexpr float const kValueStep = 0.1f;
			auto& scaleFactor = NuklearUI::g_scaleFactor;
			if (ev.isDown(KeyCode::Equals)) {
				scaleFactor += kValueStep;
			}
			if (ev.isDown(KeyCode::Hyphen)) {
				scaleFactor = Math::max(1.0f, scaleFactor - kValueStep);
			}
			if (ev.isDown(KeyCode::R)) {
				scaleFactor = 1.0f;
				_dtFactor   = 1.0f;
			}
			if (ev.isDown(KeyCode::H)) {
				_bShowUI = !_bShowUI;
			}
		}

		//_showText.clear();
		if (ev.isDown(KeyCode::W)) { _cameraDeltaPos.y = Math::clamp(_cameraDeltaPos.y+1, -kMouseMaxDelta, kMouseMaxDelta); /*_showText += "W";*/ }
		if (ev.isDown(KeyCode::S)) { _cameraDeltaPos.y = Math::clamp(_cameraDeltaPos.y-1, -kMouseMaxDelta, kMouseMaxDelta); /*_showText += "S";*/ }
		if (ev.isDown(KeyCode::A)) { _cameraDeltaPos.x = Math::clamp(_cameraDeltaPos.x+1, -kMouseMaxDelta, kMouseMaxDelta); /*_showText += "A";*/ }
		if (ev.isDown(KeyCode::D)) { _cameraDeltaPos.x = Math::clamp(_cameraDeltaPos.x-1, -kMouseMaxDelta, kMouseMaxDelta); /*_showText += "D";*/ }
		//if (_showText.size() > 0) SGE_LOG("{}\tdelta: {}", _showText, _cameraDeltaPos);
	}

private:
	void _beginRender() {
		_camera.setViewport(clientRect());

		const Vec2f& clientSize	= clientRect().size;
		_aspect = clientSize.y != 0 ? clientSize.x / clientSize.y : 0;

		glViewport(0, 0, static_cast<GLsizei>(clientSize.x), static_cast<GLsizei>(clientSize.y));
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		const auto& scaleFactor = NuklearUI::g_scaleFactor;
		glPointSize(5.0f * scaleFactor);
		glLineWidth(1.5f * scaleFactor);

		glBindVertexArray(_vertexArrayObject);
		glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_uploadWireFrameMode();
	}

	void _endRender() {
#if SGE_OS_WINDOWS
		_timingInfo.cpu->beginSwapBuffer();
#endif

		::SwapBuffers(_hdc);
		if (_vsynch != 0) {
			glFinish();
		}

#if SGE_OS_WINDOWS
		_timingInfo.cpu->endSwapBuffer();
#endif
	}

	void _cameraMove() {
		if (_cameraDeltaPos.equals0()) return;

		static const Vec2f kMouseSensitivity { 0.1f, 0.15f };

		auto d = _cameraDeltaPos * kMouseSensitivity;
		_camera.move(d.x, 0, d.y);

		if (_cameraDeltaPos.x > 0) {
			_cameraDeltaPos.x = Math::max(0.f, _cameraDeltaPos.x - kMouseDecayFactor);
		} else if (_cameraDeltaPos.x < 0) {
			_cameraDeltaPos.x = Math::min(0.f, _cameraDeltaPos.x + kMouseDecayFactor);
		}
		if (_cameraDeltaPos.y > 0) {
			_cameraDeltaPos.y = Math::max(0.f, _cameraDeltaPos.y - kMouseDecayFactor);
		} else if (_cameraDeltaPos.y < 0) {
			_cameraDeltaPos.y = Math::min(0.f, _cameraDeltaPos.y + kMouseDecayFactor);
		}
	}

	void _drawSampleSelectorWindow() {
		const auto& scaleFactor = NuklearUI::g_scaleFactor;
		const auto& clientSize  = clientRect().size;

		TempString title;
		if (_sampleType != SampleType::None) {
			FmtTo(title, "{}. {}", enumInt(_sampleType), _sampleType);
			Rect2f xywh { 0, 0, NuklearUI::Util::toNKSize(clientSize.x), 0 };
			NuklearUI::Window window(xywh, title);
			if (!window.isOpen()) {
				_exitSample();
			}
			return;
		}

		{
			Rect2f xywh { 0, 0, NuklearUI::Util::toNKSize(clientSize.x), 0 };
			NuklearUI::Window window(xywh, "My Sample Selector");
		}

		{
			static vec2f pos		{ 20.f * scaleFactor,  40.f * scaleFactor };
			static vec2f sItemSize	{ 250.f, 25.f };
			static const int sItemCount = kSampleCount - 1;
			Rect2f xywh {
				pos.x, pos.y,
				sItemSize.x + NuklearUI::LayoutRowStatic::kItemWMargin,
				sItemCount * (sItemSize.y + NuklearUI::LayoutRowStatic::kItemHMargin)
			};
			NuklearUI::Window window(xywh, "My Samples", NuklearUI::Window::kMovableStyle);
			if (!window.isOpen()) {
				return;
			}
			NuklearUI::LayoutRowStatic layout(sItemSize.y, sItemSize.x);
			for (auto flag = SampleType::None + 1; flag != SampleType::_END; flag += 1) {
				title.clear();
				FmtTo(title, "{}. {}", enumInt(flag), flag);
				NuklearUI::ButtonLabel btn(title);
				if (btn.isOpen()) {
					_switchSample(flag);
				}
			}
		}
	}
	
	void _drawSettingWindow() {
		if (!_bShowSettingWindow) return;
		auto& scaleFactor		= NuklearUI::g_scaleFactor;

#if 0 // why ???
		const Vec2f& clientSize = clientRect().size;
		float height = 120.f;
	#if 1
		Rect2f xywh { 0, clientSize.y - height * scaleFactor, clientSize.x / scaleFactor, height };
	#else
		Rect2f xywh { 0, 0, clientSize.x / scaleFactor, height };
	#endif
#endif

		static constexpr Rect2f xywh { 0, 0, 220.f, 100.f};
		NuklearUI::Window window(xywh, "ScaleFactor:");

		auto windowSize = NuklearUI::windowGetSize();
		NuklearUI::LayoutRowStatic layout(0, windowSize.x - 25.f);
		NuklearUI::SliderFloat(&scaleFactor, 1.0f, 2.0f);

		NuklearUI::Label("DeltaTime Speed:");
		NuklearUI::SliderFloat(&_dtFactor, 0.0f, 2.0f);
	}

	void _drawStatsWindow() {
		static constexpr const nk_flags kWindowStyle	= NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
		static constexpr const float kTextSingleHeight	= 20.f;
		static constexpr const float kWindowWidth		= 200.f;

		const auto& scaleFactor = NuklearUI::g_scaleFactor;
		const Vec2f& clientSize = clientRect().size;

		float imguiXPosition = clientSize.x - ( (kWindowWidth+2.f) * scaleFactor);
		float imguiYPosition = 17.f * scaleFactor;

		#define SHOW_TEXT(FORMAT, ...) \
				_showText.clear(); \
				FmtTo(_showText, FORMAT, __VA_ARGS__); \
				NuklearUI::Label(_showText); \
		// -----
		{
			imguiYPosition += 15.f * scaleFactor;
			Rect2f xywh { imguiXPosition, imguiYPosition, kWindowWidth, kTextSingleHeight * 4 };
			NuklearUI::Window window(xywh, "Display Stats", kWindowStyle);

			NuklearUI::LayoutRowStatic layout(15, 200, 1);

			SHOW_TEXT("Display Stats:\0");
			SHOW_TEXT("Display frequency: {}\0", _timingInfo.displayFrequency())
			SHOW_TEXT(_vsynch != 0 ? "VSynch: on" : "VSynch: off")
			SHOW_TEXT("Frame budget: {:0.2f} ms\0", _timingInfo.frameBudget())
			imguiYPosition += xywh.h * scaleFactor;
		}

		{
			Rect2f xywh{ imguiXPosition, imguiYPosition, kWindowWidth, kTextSingleHeight * 3 };
			NuklearUI::Window window(xywh, "High Level Timers", kWindowStyle);

			NuklearUI::LayoutRowStatic layout(15, 200, 1);

			SHOW_TEXT("High Level Timers:\0");
			NuklearUI::ScopedStyleTextColor c(_timingInfo.isSlowFrame() ? DebugDraw::kRed : DebugDraw::kGray);
			SHOW_TEXT("Frame Time: {:0.5f} ms\0", _timingInfo.frameTime());
			SHOW_TEXT("Delta Time: {:0.5f} ms\0", _timingInfo.deltaTime());
			imguiYPosition += xywh.h * scaleFactor;
		}

		{
			Rect2f xywh{ imguiXPosition, imguiYPosition, kWindowWidth, kTextSingleHeight * 3 };
			NuklearUI::Window window(xywh, "GPU Timers", kWindowStyle);

			NuklearUI::LayoutRowStatic layout(15, 200, 1);

			SHOW_TEXT("GPU Timers:\0");
			SHOW_TEXT("Game GPU: {:0.5f} ms\0", _timingInfo.appGPU());
			SHOW_TEXT("Nuklear GPU: {:0.5f} ms\0", _timingInfo.imguiGPU());
			imguiYPosition += xywh.h * scaleFactor;
		}

		{
			Rect2f xywh{ imguiXPosition, imguiYPosition, kWindowWidth, kTextSingleHeight * 6 };
			NuklearUI::Window window(xywh, "CPU Timers", kWindowStyle);
			NuklearUI::LayoutRowStatic layout(15, 200, 1);

			SHOW_TEXT("CPU Timers:\0");
			//SHOW_TEXT("Win32 Events: {:0.5f} ms\0", _timingInfo.win32Events());
			SHOW_TEXT("Game Update: {:0.5f} ms\0",  _timingInfo.frameUpdate());
			SHOW_TEXT("Game Render: {:0.5f} ms\0",  _timingInfo.frameRender());
			SHOW_TEXT("Nuklear Logic: {:0.5f} ms\0",  _timingInfo.imguiLogic());
			SHOW_TEXT("Nuklear Render: {:0.5f} ms\0", _timingInfo.imguiRender());
			SHOW_TEXT("Swap Buffers: {:0.5f} ms\0", _timingInfo.swapBuffer());
			imguiYPosition += xywh.h * scaleFactor;
		}
		#undef SHOW_TEXT
	}

	void _drawUI() {
		if (!_bShowUI) return;

		{
#if SGE_OS_WINDOWS
			_timingInfo.cpu->beginImGuiUpdate();
#endif
			auto scopedWireFrame = ScopedValue_make(_bWireFrame, false); SGE_UNUSED(scopedWireFrame);
			_uploadWireFrameMode();

			if (_sampleContext)
				_sampleContext->drawUI(_sampleRequest);

			_drawSampleSelectorWindow();
			_drawSettingWindow();
			_drawStatsWindow();

#if SGE_OS_WINDOWS
			_timingInfo.cpu->endImGuiUpdate();

			_timingInfo.cpu->beginImGuiRender();
			_timingInfo.gpu->beginImGuiRender();
#endif

			NuklearUI::render(clientRect().size);

#if SGE_OS_WINDOWS
			_timingInfo.gpu->endImGuiRender();
			_timingInfo.cpu->endImGuiRender();
#endif
		}
		_uploadWireFrameMode();
	}

	void _uploadWireFrameMode() {
		glPolygonMode(GL_FRONT_AND_BACK, _bWireFrame ? GL_LINE : GL_FILL);
	}

	void _switchSample(SampleType newType) {
		if (_sampleType == newType)
			return;

		_sampleType = newType;
		_createSample();
	}

	void _exitSample() {
		_switchSample(SampleType::None);
	}

	void _switchToLastSample() {
		int sampleIndex = (enumInt(_sampleType) - 1 + kSampleCount) % kSampleCount;
		_switchSample(static_cast<SampleType>(sampleIndex));
	}

	void _switchToNextSample() {
		int sampleIndex = (enumInt(_sampleType) + 1) % kSampleCount;
		_switchSample(static_cast<SampleType>(sampleIndex));
	}

	void _destroySample() {
		if (_sampleContext) {
			_sampleContext.release();
			_sampleContext = nullptr;
		}
	}

	void _createSample() {
		_destroySample();
		_sampleContext = make_unique<SampleContext>();
		_sampleRequest.reset(_sampleType);
		_sampleContext->create(_sampleRequest);
	}

	HDC		_hdc				= nullptr;

	GLuint	_vertexArrayObject	= 0;
	int		_vsynch				= 0;

	float	_aspect				= 0;
	bool	_bShowUI			= true;
	bool	_bShowSettingWindow	= false;
	bool	_bWireFrame			= false;
	float	_dtFactor			= 1.0f;

	Math::Camera3f		_camera;
	Vec2f				_cameraDeltaPos {0,0};

	SampleType			_sampleType	= SampleType::None;
	SampleRequest		_sampleRequest { _bWireFrame, _camera };
	UPtr<SampleContext>	_sampleContext;

#if SGE_OS_WINDOWS
	TempString			_showText;
public:
	TimingInfo			_timingInfo;
#endif
};

class GameAnimeProgApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test102");
		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1500, 900 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Game Anime Prog Window - OpenGL");
		}
	}

	virtual void onUpdate(float dt) override {
#if SGE_OS_WINDOWS
		auto& ti = _mainWin._timingInfo;
		ti.cpu->beginFrame();
#endif

		_mainWin.update(dt);
		_mainWin.render();
		NuklearUI::UIInput input; // tricky

#if SGE_OS_WINDOWS
		ti.cpu->endFrame();
		ti.profilingHouseKeeping();
#endif
	}

private:
	GameAnimeProgMainWin _mainWin;
};

} // namespace sge


SGE_MAIN(sge::GameAnimeProgApp)