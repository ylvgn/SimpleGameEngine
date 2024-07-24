#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
public:
	virtual void onInitGL() override {
		Base::onInitGL();

		_wireframe = true;
		_simpleColorShader = new MyShader();
		_simpleLightingShader = new MyShader();

		{
			MyEditMesh editMesh;
//			MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test.obj");
			MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test2.obj");
			editMesh.setColor({ 255, 255, 255, 255 });
			_renderMesh.create(editMesh);
		}

		{
			MyEditMesh editMesh;
			editMesh.createGrid(20, 20, 1, { 255, 255, 255, 255 });
			_gridMesh.create(editMesh);
		}

		{
			MyEditMesh editMesh;
			editMesh.createOrigin();
			_originMesh.create(editMesh);
		}
		
		_simpleColorShader->loadFile("Assets/Shaders/simpleColor");
		_simpleLightingShader->loadFile("Assets/Shaders/simpleLighting");
	}

	virtual void onDraw() override {
		Base::onDraw();
		auto uptime = static_cast<float>(_uptime.get());

		beginRender();

		example1(uptime);

		endRender();
	}

	void example1(float uptime) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		_simpleColorShader->bind();
		{
			_simpleColorShader->setUniform("matMVP", _matrix_vp);

			_simpleColorShader->draw(_gridMesh);
			_simpleColorShader->draw(_originMesh);
		}
		_simpleColorShader->unbind();

		_simpleLightingShader->bind();
		{
			_simpleLightingShader->setUniform("uptime", uptime);
			_simpleLightingShader->setUniform("matMVP", _matrix_vp);
			_simpleLightingShader->setUniform("lightPos", Vec3f(1,2,2));

			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-1.f, 0.001f);
				_simpleLightingShader->draw(_renderMesh);
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		_simpleLightingShader->unbind();

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;
		
		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Right: {
					_simpleColorShader->reload();
					_simpleLightingShader->reload();
				}break;
				case Button::Left: {
					_wireframe = !_wireframe;
				}break;
			}
		}
	}

private:
	bool _wireframe : 1;

	MyHiResTimer	_uptime;

	MyRenderMesh	_gridMesh;
	MyRenderMesh	_originMesh;

	MyRenderMesh	_renderMesh;

	SPtr<MyShader>	_simpleColorShader;
	SPtr<MyShader>	_simpleLightingShader;
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
