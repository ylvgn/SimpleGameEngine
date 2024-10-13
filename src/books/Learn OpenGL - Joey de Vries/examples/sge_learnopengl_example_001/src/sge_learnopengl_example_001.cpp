#include <sge_learnopengl.h>

namespace sge {
/*
	* Hello Window
	* FYI: https://learnopengl.com/Getting-started/Hello-Window
*/

class sge_learnopengl_example_001 : public MyGLFWNativeUIWindow {
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

SGE_GLFW_MAIN(sge::sge_learnopengl_example_001)
} // namespace sge