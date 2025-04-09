#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
public:
	virtual void onInitGL() override {
		Base::onInitGL();

		_wireframe = true;

		_simpleColorShader = SPtr_make<MyShader>();
		_simpleLightingShader = SPtr_make<MyShader>();
	//-----
		_simpleColorCgShader = SPtr_make<MyShader>();
		_simpleLightingCgShader = SPtr_make<MyShader>();

		{
			MyEditMesh editMesh;
//			MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test.obj");
			MyWavefrontObjLoader::loadFile(editMesh, "Assets/Models/test2.obj");
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
		
		_simpleColorShader->loadFile("Assets/Shaders/simpleColor");
		_simpleLightingShader->loadFile("Assets/Shaders/simpleLighting");
	//-----
		_simpleColorCgShader->loadFile("Assets/Shaders/simpleColor.cg");
		_simpleLightingCgShader->loadFile("Assets/Shaders/simpleLighting.cg");
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;
		
		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Right: {
					_simpleColorShader->reload();
					_simpleLightingShader->reload();
				//-----
					_simpleColorCgShader->reload();
					_simpleLightingCgShader->reload();
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
		Mat4f matrix_model = Mat4f::s_identity();
		auto mvp = _matrix_vp * matrix_model;
		{
			auto& s = _simpleColorShader;
			s->bind();
				s->setUniform("matMVP", mvp);

				s->draw(_gridMesh);
				s->draw(_originMesh);
			s->unbind();
		}

		{
			auto& s = _simpleLightingShader;
			s->bind();
				s->setUniform("uptime", uptime);
				s->setUniform("matMVP", mvp);
				s->setUniform("lightPos", Vec3f(1,2,2));

				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(-1.f, 0.001f);
					s->draw(_renderMesh);
				glDisable(GL_POLYGON_OFFSET_FILL);
			s->unbind();
		}
	}

	void example2(float uptime) {
		Mat4f matrix_model = Mat4f::s_identity();
		auto mvp = _matrix_vp * matrix_model;
		{
			auto& s = _simpleColorCgShader;
			s->bind();
				s->setUniformCg("_matMVP", mvp);

				s->drawCg(_gridCgMesh);
				s->drawCg(_originCgMesh);
			s->unbind();
		}

		{
			auto& s = _simpleLightingCgShader;
			s->bind();
				s->setUniformCg("_matMVP", mvp);
				s->setUniform("_lightPos", Vec3f(-1,2,2));
				s->setUniform("_uptime", uptime);

				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(-1.f, 0.001f);
					s->drawCg(_renderCgMesh);
				glDisable(GL_POLYGON_OFFSET_FILL);
			s->unbind();
		}
	}

private:
	bool _wireframe : 1;

	MyHiResTimer	_uptime;

//-----
	MyRenderMesh	_gridMesh;
	MyRenderMesh	_originMesh;
	MyRenderMesh	_renderMesh;

	SPtr<MyShader>	_simpleColorShader;
	SPtr<MyShader>	_simpleLightingShader;

//-----
	MyRenderMesh	_gridCgMesh;
	MyRenderMesh	_originCgMesh;
	MyRenderMesh	_renderCgMesh;

	SPtr<MyShader>	_simpleColorCgShader;
	SPtr<MyShader>	_simpleLightingCgShader;
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