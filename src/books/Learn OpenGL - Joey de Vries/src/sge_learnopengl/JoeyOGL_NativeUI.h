#pragma once

#include "JoeyOGL_Common.h"

namespace sge {

class JoeyOGL_NativeUIWindow : public NativeUIWindow {
	using Base = NativeUIWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

		{ // setup camera
			_camera.setPos(0, 10, 10);
			_camera.setAim(0, 0, 0);
		}
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		if (_renderContext->onUIMouseEvent(ev))
			return;

		if (ev.isDragging()) {
			using Button = UIMouseEventButton;
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
			auto d = ev.scroll * -0.005f;
			_camera.dolly(d.y);
		}
	}

	virtual void onCloseButton() override {
		NativeUIApp::current()->quit(0);
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		_renderContext->onUIKeyboardEvent(ev);
	}

	Math::Camera3f			_camera;
	SPtr<RenderContext>		_renderContext;
	RenderRequest			_renderRequest;

	Scene					_scene;
	EditorHierarchyWindow	_hierarchyWindow;
	EditorInspectorWindow	_inspectorWindow;
	EditorStatisticsWindow	_statisticsWindow;
};

template<class MainWin, class Enable = std::enable_if_t< TypeTraits::isBaseOf<JoeyOGL_NativeUIWindow, MainWin>::value > >
class JoeyOGL_NativeUIApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test101");

//		CommandLine::runShell("Assets/Shaders/sge_ninja.bat"); //some bug when file already exists will not force build after modified.
		CommandLine::runShell("Assets/Shaders/sge_gnu_make.bat");

		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			renderDesc.apiType = Renderer::ApiType::OpenGL; // pbr in OpenGL still has some bug
//			renderDesc.apiType = Renderer::ApiType::DX11;
			Renderer::create(renderDesc);
		}

		EditorContext::create();
		EngineContext::create();

		{ // create window
			TempString title;
			FmtTo(title, "Learn OpenGL - {}, VSync: {}", Renderer::instance()->apiType(), Renderer::instance()->vsync());

			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1040, 880 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle(title);
		}
	}

	virtual void onQuit() {
		EngineContext::destroy();
		EditorContext::destroy();
		Base::onQuit();
	}

private:
	MainWin	_mainWin;
};

} // namespace sge