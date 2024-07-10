#include <sge_sita_ray_tracer.h>

namespace sge {

class TestRayCastWindow : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
protected:
	virtual void onInitGL() override {
		Base::onInitGL();
		_mesh.loadObjFile("Assets/models/sphere_smooth.obj");

		_debugRay.origin = MyVec3f::s_zero();
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
		drawRaycastResult();

		swapBuffers();
		drawNeeded();
	}

	void rayTracing(float x, float y) {
		MyMat4f projMatrix;
		MyMat4f modelview;

		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix.data);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview.data);

		MyVec2f screenSize = _frameBufferSize;

		_rayTraser.init(screenSize, projMatrix, modelview);

		MyRay3f ray = _rayTraser.getRay(x, y);

		_debugRay = ray;

		{
			//Math::MyPlane3f plane(MyVec3f(0,1,0), 0.5f);
			//ray.raycast(_hitResult, plane);
		}

		{
			Math::MySphere3f sphere(MyVec3f(0, 0, 0), 1.f);
			ray.raycast(_hitResult, sphere);
		}
	}

	void drawDebugRay() {
		drawDebugRay(_debugRay);
	}

	void drawRaycastResult() {
		drawRaycastResult(_hitResult);
	}

	void drawRaycastResult(const MyRay3f::HitResult& result, const Color4f& c = { 1,0,0,1 }, float lineWidth = 1) {
		if (!result.hasResult)
			return;

		glLineWidth(lineWidth);

		auto pt = result.point;
		auto nl = result.normal;

		glColor4f(c.r, c.g, c.g, c.a);
			glBegin(GL_POINTS);
			glVertex3fv(pt.data);
		glEnd();

		glBegin(GL_LINES);
			glVertex3fv(pt.data);
			glVertex3fv((pt + nl).data);
		glEnd();
	}

	void drawDebugRay(MyRay3f ray, const Color4f& c = {1,0,1,1}, float lineWidth = 1) {
		if (!ray.dir.sqrMagnitude())
			return;

		glLineWidth(lineWidth);
		auto start = ray.origin;
		auto end = ray.origin + ray.dir;

		glColor4f(c.r, c.g, c.g,c.a);
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

	MyRay3f::HitResult _hitResult;
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
