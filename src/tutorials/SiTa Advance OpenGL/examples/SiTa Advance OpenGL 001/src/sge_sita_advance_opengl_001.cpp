#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
public:
	virtual void onInitGL() override {
		Base::onInitGL();

		_wireframe = true;
		_shader    = new MyShader();

		MyWavefrontObjLoader::loadFile(_editMesh, "Assets/Models/test.obj");
		_editMesh.setColor({ 255, 255, 255, 255 });
		_editMesh.updateIndex();

		_shader->loadFile("Assets/Shaders/test001");
	}

	virtual void onDraw() override {
		Base::onDraw();

		beginRender();

		example1();

		endRender();
	}

	void example1() {
		my_drawGrid();
		my_drawOriginAxis();

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		_shader->bind();
		{
			_shader->setUniform("matMVP", _matrix_vp);
			_editMesh.draw();
		}
		_shader->unbind();

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;
		
		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Right: _shader->reload(); break;
				case Button::Left: _wireframe = !_wireframe; break;
			}
		}
	}

private:
	bool _wireframe : 1;

	MyEditMesh		_editMesh;
	SPtr<MyShader>	_shader;
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
