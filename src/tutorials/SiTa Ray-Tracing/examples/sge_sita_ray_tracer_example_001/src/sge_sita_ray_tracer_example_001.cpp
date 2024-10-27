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

	void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;

		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Left: {
					rayTracing(ev.pos.x, ev.pos.y);
				}break;
			}
		}
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

	void rayTracing(float x, float y) {
		glGetFloatv(GL_PROJECTION_MATRIX, _tmpProjMatrix.data);
		glGetFloatv(GL_MODELVIEW_MATRIX, _tmpModelview.data);

		_rayTracer.init(_frameBufferSize, _tmpProjMatrix, _tmpModelview);

		_debugRay = _rayTracer.getRay(x, y);
	}

private:
	MyMat4f			_tmpProjMatrix;
	MyMat4f			_tmpModelview;

	MyRay3f			_debugRay;
	MyRayTracerf	_rayTracer;
};

class MyApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../../../../../../examples/Test106");

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
