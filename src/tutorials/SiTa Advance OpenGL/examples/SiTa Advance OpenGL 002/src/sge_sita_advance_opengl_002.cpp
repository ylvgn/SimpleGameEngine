#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
public:
	virtual void onInitGL() override {
		Base::onInitGL();

		_wireframe  = true;
		_simpleColorShader = new MyShader();

		{
			MyEditMesh editMesh;
			MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test.obj");
			editMesh.setColor({ 255, 0, 255, 255 });
			_renderMesh.updateVBO(editMesh);
		}

		{
			MyEditMesh editMesh;
			editMesh.createGrid(20, 20, 1, { 255, 255, 255, 255 });
			_gridMesh.updateVBO(editMesh);
		}

		{
			MyEditMesh editMesh;
			editMesh.createOrigin();
			_originMesh.updateVBO(editMesh);
		}
		
		_simpleColorShader->loadFile("Assets/Shaders/simpleColor");
	}

	virtual void onDraw() override {
		Base::onDraw();

		beginRender();

		_simpleColorShader->bind();

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		{
			_simpleColorShader->setUniform("matMVP", _matrix_vp);

			_simpleColorShader->draw(_gridMesh);
			_simpleColorShader->draw(_originMesh);

			_simpleColorShader->draw(_renderMesh);
		}

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		_simpleColorShader->unbind();

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
					_simpleColorShader->reload();
				}break;
				case Button::Left: {
					_wireframe = !_wireframe;
				}break;
			}
		}
	}

private:
	bool _wireframe : 1;

	MyRenderMesh	_gridMesh;
	MyRenderMesh	_originMesh;

	MyRenderMesh	_renderMesh;

	SPtr<MyShader>	_simpleColorShader;
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
