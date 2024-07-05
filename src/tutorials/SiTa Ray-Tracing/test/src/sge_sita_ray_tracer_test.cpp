#include <sge_sita_ray_tracer.h>

namespace sge {

class TestRayCastWindow : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
protected:
	virtual void onInitGL() override {
		Base::onInitGL();
		_mesh.loadObjFile("Assets/models/sphere_smooth.obj");

		_debugRay.origin.set(MyVec3f::s_zero());
		_debugRay.dir = MyVec3f::s_zero();
	}

	virtual void onDraw() override {
		setFrameBufferSize(_clientRect.size);
		
		drawGrid();
		drawOriginAxis();

		glPointSize(10);

		_mesh.wireframe = true;
		_mesh.draw();

		drawDebugRay();

		swapBuffers();
		drawNeeded();
	}

	void rayTracing(float x, float y) {
		MyMat4f projMatrix;
		MyMat4f modelview;

		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix.data);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data);

		MyVec2f screenSize{ _frameBufferSize.x, _frameBufferSize.y };

		_rayTraser.init(screenSize, projMatrix, modelview);

		MyRay3f ray = _rayTraser.getRay(x, y);

		_debugRay = ray;
	}

	void drawDebugRay() {
		if (!_debugRay.dir.sqrMagnitude()) return;

		auto start = _debugRay.origin;
		auto end = _debugRay.origin + _debugRay.dir;

		glColor4f(1,0,1,1);
		glBegin(GL_POINTS);
			glVertex3fv(start.data);
		glEnd();

		glBegin(GL_LINES);
			glVertex3fv(start.data);
			glVertex3fv(end.data);
		glEnd();
	}

	void onUIMouseEvent(UIMouseEvent& ev) {
		Base::onUIMouseEvent(ev);

		using Button	= UIMouseEvent::Button;
		using Type		= UIMouseEvent::Type;
		using Modifier	= UIEventModifier;

		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Left: {
					rayTracing(ev.pos.x, ev.pos.y);
				}break;
			}
		}
	}

private:
	MyMesh		_mesh;
	MyRayTracer _rayTraser;
	MyRay3f		_debugRay;
};

class SiTaRayTracerTestApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../examples/Test106");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1376, 768 };
			_testRayWin.create(winDesc);
		}
	}

private:
	TestRayCastWindow _testRayWin;
};

}

int main(int argc, const char** argv) {
	sge::SiTaRayTracerTestApp app;
	sge::SiTaRayTracerTestApp::CreateDesc desc;
	app.run(desc);

	return 0;
}
