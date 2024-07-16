#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
public:
	virtual void onInitGL() override {
		Base::onInitGL();

		wireframe = true;
		_shader   = new MyShader();

		MyEditMesh editMesh;
		MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test.obj");
		for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
			editMesh.color.emplace_back(255, 255, 255, 255);
		}

		_renderMesh.create(editMesh);
		_shader->loadFile("Assets/Shaders/test001");
	}

	virtual void onDraw() override {
		Base::onDraw();

		beginRender();
		_shader->bind();

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		{
			_shader->setUniform("matMVP", _matrix_vp);
			_renderMesh.draw();
		}

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		_shader->unbind();
		endRender();
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button	= UIMouseEvent::Button;
		using Type		= UIMouseEvent::Type;
		using Modifier	= UIEventModifier;
		
		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Right: {
					_shader->reload();
				}break;
			}
		}
	}

	bool wireframe : 1;

private:
	MyRenderMesh		_renderMesh;
	SPtr<MyShader>		_shader;
};

class MyApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../../examples/Test107");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1376, 768 };
			_mainWin.create(winDesc);
		}
	}

private:
	MyMainWin _mainWin;
};

}

int main() {
	sge::MyApp app;
	sge::MyApp::CreateDesc desc;
	app.run(desc);

	return 0;
}
