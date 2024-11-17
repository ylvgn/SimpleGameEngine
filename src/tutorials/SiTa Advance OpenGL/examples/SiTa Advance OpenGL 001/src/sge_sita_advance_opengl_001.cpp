#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
public:
	virtual void onInitGL() override {
		Base::onInitGL();

		_wireframe = true;

		_testShader = new MyShader();
		_testCgShader = new MyShader();

		MyWavefrontObjLoader::loadFile(_editMesh, "Assets/Models/test.obj");
		_editMesh.setColor({ 255, 255, 255, 255 });
		_editMesh.updateIndex();

		_testShader->loadFile("Assets/Shaders/test001");
		_testCgShader->loadFile("Assets/Shaders/test001.cg");
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;
		
		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Right: {
					_testShader->reload();
					_testCgShader->reload();
				}break;
				case Button::Left: {
					_wireframe = !_wireframe;
				}break;
			}
		}
	}

	virtual void onDraw() override {
		Base::onDraw();

		beginRender();

		my_drawGrid();
		my_drawOriginAxis();

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//example1();
		example2();

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		endRender();
	}

	void example1() {
		auto& s = _testShader;
		s->bind();
			s->setUniform("matMVP", _matrix_vp);
			_editMesh.draw();
		s->unbind();
	}

	void example2() {
		auto& s = _testCgShader;
		s->bind();
			s->setUniformCg("_matMVP", _matrix_vp);
			_editMesh.draw();
		s->unbind();
	}

private:
	bool _wireframe : 1;

	MyEditMesh		_editMesh;

	SPtr<MyShader>	_testShader;
	SPtr<MyShader>	_testCgShader;
};

class MyApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test107");
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

} // namespace sge


SGE_MAIN(sge::MyApp)
