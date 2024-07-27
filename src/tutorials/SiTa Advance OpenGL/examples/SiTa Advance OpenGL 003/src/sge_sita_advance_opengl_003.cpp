#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
public:
	virtual void onInitGL() override {
		Base::onInitGL();

		_wireframe = true;

		_simpleColorShader = new MyShader();
		_simpleTextureShader = new MyShader();
	//----
		_simpleColorCgShader = new MyShader();
		_simpleCgTextureShader = new MyShader();

		{
			MyEditMesh editMesh;
			MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test.obj");
//			MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test2.obj");
			editMesh.setColor({ 255, 255, 255, 255 });
			_renderMesh.create(editMesh);
			_renderCgMesh.createCg(editMesh);
		}

		{
			MyEditMesh editMesh;
			editMesh.createGrid(20, 20, 1, { 255, 255, 255, 255 });
			_gridMesh.create(editMesh);
			_gridCgMesh.createCg(editMesh);
		}

		{
			MyEditMesh editMesh;
			editMesh.createOrigin();
			_originMesh.create(editMesh);
			_originCgMesh.createCg(editMesh);
		}

		_tex0.loadPngFile("Assets/Textures/Kingfisher.png", false);
		_tex1.loadPngFile("Assets/Textures/happymask.png", false);

		_simpleColorShader->loadFile("Assets/Shaders/simpleColor");
		_simpleTextureShader->loadFile("Assets/Shaders/simpleTexture");
	//----
		_simpleColorCgShader->loadFile("Assets/Shaders/simpleColor.cg");
		_simpleCgTextureShader->loadFile("Assets/Shaders/simpleTexture.cg");
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;
		
		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Right: {
					_simpleColorShader->reload();
					_simpleTextureShader->reload();
				//----
					_simpleColorCgShader->reload();
					_simpleCgTextureShader->reload();
				}break;
				case Button::Left: {
					_wireframe = !_wireframe;
				}break;
			}
		}
	}

	virtual void onDraw() override {
		Base::onDraw();
		auto uptime = static_cast<float>(_uptime.get());

		beginRender();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//example1(uptime);
		example2(uptime);

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		endRender();
	}

	void example1(float uptime) {
		{
			auto& s = _simpleColorShader;
			s->bind();
				s->setUniform("matMVP", _matrix_vp);

				s->draw(_gridMesh);
				s->draw(_originMesh);
			s->unbind();
		}

		{
			auto& s = _simpleTextureShader;
			s->bind();
				s->setUniform("matMVP", _matrix_vp);
				s->setUniform("sampler0", _tex0);
				s->setUniform("sampler1", _tex1);
				s->setUniform("uptime", uptime);

				s->draw(_renderMesh);
			s->unbind();
		}
	}

	void example2(float uptime) {
		{
			auto& s = _simpleColorCgShader;
			s->bind();
				s->setUniformCg("_matMVP", _matrix_vp);

				s->drawCg(_gridCgMesh);
				s->drawCg(_originCgMesh);
			s->unbind();
		}

		{
			auto& s = _simpleCgTextureShader;
			s->bind();
				s->setUniformCg("_matMVP", _matrix_vp);
				s->setUniform("_uptime", uptime);
				s->setUniform("_sampler0", _tex0);
				s->setUniform("_sampler1", _tex1);

				s->drawCg(_renderCgMesh);
			s->unbind();
		}
	}

private:
	bool _wireframe : 1;

	MyHiResTimer	_uptime;

	MyTexture2D		_tex0;
	MyTexture2D		_tex1;

//-----
	MyRenderMesh	_gridMesh;
	MyRenderMesh	_originMesh;
	MyRenderMesh	_renderMesh;

	SPtr<MyShader>	_simpleColorShader;
	SPtr<MyShader>	_simpleTextureShader;

//-----
	MyRenderMesh	_gridCgMesh;
	MyRenderMesh	_originCgMesh;
	MyRenderMesh	_renderCgMesh;

	SPtr<MyShader>	_simpleColorCgShader;
	SPtr<MyShader>	_simpleCgTextureShader;
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
