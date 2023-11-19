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

// V Synch
typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL		(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int			(WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

class GameAnimeProgMainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	using Type = MySampleType;

	static constexpr const int		kSampleCount		= enumInt(Type::_END);
	static constexpr const float	kMouseDecayFactor	= 0.3f;
	static constexpr const float	kMouseMaxDelta		= 10.f;

	void _cameraMove() {
		if (_cameraDeltaPos.equals0()) return;

		static const Vec2f  kMouseSensitivity { 0.1f, 0.15f };

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

#if SGE_OS_WINDOWS
		_timingInfo.cpu->endFrame();
		_timingInfo.profilingHouseKeeping();
		_timingInfo.cpu->beginFrame();
#endif
	}

protected:
	virtual void onCreate(CreateDesc& desc) override  {
		Base::onCreate(desc);

		{ // create opengl render context
			const HDC dc = hdc();

			PIXELFORMATDESCRIPTOR pfd;
			pfd 				= {};
			pfd.nSize 			= sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion 		= 1;
			pfd.dwFlags 		= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.iPixelType 		= PFD_TYPE_RGBA;
			pfd.cColorBits 		= 24;
			pfd.cDepthBits 		= 32;
			pfd.cStencilBits 	= 8;
			pfd.iLayerType 		= PFD_MAIN_PLANE;

			int pixelFormat 	= ChoosePixelFormat(dc, &pfd);
			SetPixelFormat(dc, pixelFormat, &pfd);

			// legacy render context
			HGLRC tempRC = wglCreateContext(dc);
			wglMakeCurrent(dc, tempRC);

			// legacy render context just for get function pointer of 'wglCreateContextAttribsARB'
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
			const int attribList[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_FLAGS_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB,
				WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};

			// modern render context
			HGLRC hglrc = wglCreateContextAttribsARB(dc, 0, attribList);

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tempRC);
			wglMakeCurrent(dc, hglrc);

			// use 'glad' to load all opengl core function
			if (!gladLoadGL()) {
				throw SGE_ERROR("Could not initialize GLAD\n");
			}

			SGE_LOG("OpenGL Version: {}.{} loaded", GLVersion.major, GLVersion.minor);
		}

		{ // vsynch: https://www.khronos.org/opengl/wiki/Swap_Interval
			PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(wglGetProcAddress("wglGetExtensionsStringEXT"));
			bool isSwapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

			_vsynch = 0;
			if (isSwapControlSupported) {
				PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
				PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(wglGetProcAddress("wglGetSwapIntervalEXT"));

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
		}

		// Setup some OpenGL required state
		glGenVertexArrays(1, &_vertexArrayObject);
		glBindVertexArray(_vertexArrayObject);

#if SGE_OS_WINDOWS
		_timingInfo.create(_hwnd);
		_timingInfo.cpu->beginFrame();
#endif

		// set-up camera
		_camera.setPos(0, 10, 10);
		_camera.setAim(0, 0, 0);

		// create Nuklear (imgui)
		NuklearUI::createContext();

		// create sample
		_createSample();
	}

	virtual void onCloseButton() override {
		if (_vertexArrayObject != 0) {
			HDC dc		= hdc();
			HGLRC hglrc = wglGetCurrentContext();

			// delete VAO
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &_vertexArrayObject);
			_vertexArrayObject = 0;

			// deleate Nuklear
			NuklearUI::destroyContext();

			// delete render context
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(_hwnd, dc);
		}

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

		SwapBuffers(hdc());
		if (_vsynch != 0) {
			glFinish();
		}

#if SGE_OS_WINDOWS
		_timingInfo.cpu->endSwapBuffer();
		_timingInfo.cpu->endFrame();
#endif

	}

	void _drawSampleSelectorWindow() {
		const auto& scaleFactor = NuklearUI::g_scaleFactor;
		const auto& clientSize  = clientRect().size;

		TempString title;
		if (_type != Type::None) {
			FmtTo(title, "{}. {}", enumInt(_type), _type);
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
			for (auto flag = Type::None + 1; flag != Type::_END; flag += 1) {
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
			auto bWithoutWireFrame = makeScopedValue(&_bWireFrame, false);
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

	void _switchSample(Type newType) {
		if (_type == newType)
			return;

		_type = newType;
		_createSample();
	}

	void _exitSample() {
		_switchSample(Type::None);
	}

	void _switchToLastSample() {
		int sampleIndex = (enumInt(_type) - 1 + kSampleCount) % kSampleCount;
		_switchSample(static_cast<Type>(sampleIndex));
	}

	void _switchToNextSample() {
		int sampleIndex = (enumInt(_type) + 1) % kSampleCount;
		_switchSample(static_cast<Type>(sampleIndex));
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
		_sampleRequest.reset();
		_sampleContext->create(_sampleRequest);
	}

	GLuint	_vertexArrayObject	= 0;
	int		_vsynch				= 0;

	Type	_type				= SampleContext::kStartUpDefaultType;
	float	_aspect				= 0;
	bool	_bWireFrame			= false;

	bool	_bShowUI			= true;
	bool	_bShowSettingWindow	= false;
	float	_dtFactor			= 1.0f;

	Math::Camera3f		_camera;
	Vec2f				_cameraDeltaPos {0,0};

	SampleRequest		_sampleRequest { _type, _bWireFrame, _camera };
	UPtr<SampleContext>	_sampleContext;

#if SGE_OS_WINDOWS
	TimingInfo			_timingInfo;
	TempString			_showText;
#endif
};

class GameAnimeProgApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../examples/Test102");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1500, 900 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Game Anime Prog Window");
		}
	}

	virtual void onUpdate(float dt) override {
		_mainWin.update(dt);
		_mainWin.render();
		NuklearUI::UIInput input; // tricky
	}

private:
	GameAnimeProgMainWin _mainWin;
};

} // namespace


int main(int argc, const char** argv) {
	sge::GameAnimeProgApp app;
	sge::GameAnimeProgApp::CreateDesc desc;
	app.run(desc);

	return 0;
}
