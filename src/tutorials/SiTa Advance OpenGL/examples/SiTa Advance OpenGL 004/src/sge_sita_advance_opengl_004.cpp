#include <sge_sita_advance_opengl.h>

namespace sge {

class MyMainWin : public MyImGuiUIWindow {
	using This = MyMainWin;
	using Base = MyImGuiUIWindow;
public:
	MyMainWin(CreateDesc& desc)
		: Base(desc)
	{
		_wireframe = false;

		_simpleColorShader = new MyShader();
		_simpleColorShader->loadFile("Assets/Shaders/simpleColor");

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
	}

	virtual bool onWindowCloseButton() override {
		MyImGuiApp::current()->quit(0);
		return true;
	}

	virtual void onRender() override {
		Base::onRender();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		example1();
		ImGui::ShowDemoWindow(nullptr);

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	virtual void onUpdate(float dt) override {
		Base::onUpdate(dt);
	}

	void example1() {
		Mat4f matrix_model = Mat4f::s_identity();
		auto* win = getNativeWindow();
		auto mvp = win->_matrix_vp * matrix_model;
		{
			auto& s = _simpleColorShader;
			s->bind();
				s->setUniform("matMVP", mvp);

				s->draw(_gridMesh);
				s->draw(_originMesh);
			s->unbind();
		}
	}

	MyRenderMesh	_gridMesh;
	MyRenderMesh	_originMesh;

	SPtr<MyShader>	_simpleColorShader;

	bool _wireframe : 1;
};

class MyApp : public MyImGuiApp {
	using Base = MyImGuiApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test107");
		Base::onCreate(desc);

		{ // create window
			SGE_ASSERT(_mainWin == nullptr);
			MyMainWin::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1376, 768 };
			_mainWin = new MyMainWin(winDesc);
		}
	}
};

} // namespace sge


SGE_MAIN(sge::MyApp)