#include <sge_sita_ray_tracer.h>

namespace sge {

class MainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
protected:
	virtual void onInitGL() override {
		Base::onInitGL();

		_debugRay.origin = MyVec3f::s_zero();
		_debugRay.dir = MyVec3f::s_zero();
	}

	virtual void onDraw() override {
		Base::onDraw();

		setFrameBufferSize(_clientRect.size);
		my_drawGrid();
		my_drawOriginAxis();

		glPointSize(10);
		_debugRay.draw();

		swapBuffers();
		drawNeeded();
	}

	void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;

		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Left: {
					rayTracing(ev.pos.x, ev.pos.y);
				} break;
			}
		}
	}

	void rayTracing(float x, float y) {
		glGetFloatv(GL_PROJECTION_MATRIX, _tmpProjMatrix.data);
		glGetFloatv(GL_MODELVIEW_MATRIX, _tmpModelview.data);

		const auto& screenSize = _frameBufferSize;
		_rayTraser.init(screenSize, _tmpProjMatrix, _tmpModelview);

		_debugRay = _rayTraser.getRay(x, y);
	}

private:
	MyRay3f		_debugRay;
	MyRayTracer	_rayTraser;

	MyMat4f		_tmpProjMatrix;
	MyMat4f		_tmpModelview;
};

class MyApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../../examples/Test106");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1376, 768 };
			_mainWin.create(winDesc);
		}
	}

private:
	MainWin _mainWin;
};

}

int main(int argc, const char** argv) {
	sge::MyApp app;
	sge::MyApp::CreateDesc desc;
	app.run(desc);

	return 0;
}