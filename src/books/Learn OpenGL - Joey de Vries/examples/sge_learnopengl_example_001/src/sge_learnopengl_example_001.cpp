#include <sge_learnopengl.h>

namespace sge {
/*
	* Hello Window
	* FYI:
		* 1.1.hello_window: https://learnopengl.com/Getting-started/Creating-a-window
		* 1.2.hello_window_clear: https://learnopengl.com/Getting-started/Hello-Window
*/

class MainWin : public MyGLFWNativeUIWindow {
	using Base = MyGLFWNativeUIWindow;
protected:
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		Base::onUIKeyboardEvent(ev);

		if (ev.isDown(KeyCode::Escape)) {
			MyGLFWNativeUIApp::current()->quit(0);
		}
	}

	virtual void onFramebufferSizeCallback(int width, int height) override {
		Base::onFramebufferSizeCallback(width, height);

		glViewport(0, 0, width, height);
	}

	virtual void onRender() override {
		Base::onRender();

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	}
};

class MyApp : public MyGLFWNativeUIApp {
	using Base = MyGLFWNativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		_mainWin = eastl::make_unique<MainWin>();

		{
			MainWin::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1376, 768 };
			_mainWin->create(winDesc);
			_mainWin->setWindowTitle("1.getting_started - Hello Window");
		}

		Base::onCreate(desc);
	}
};

} // namespace sge

int main(int argc, const char** argv) {
	sge::MyApp app;
	sge::MyApp::CreateDesc desc;
	app.run(desc);

	return 0;
}
