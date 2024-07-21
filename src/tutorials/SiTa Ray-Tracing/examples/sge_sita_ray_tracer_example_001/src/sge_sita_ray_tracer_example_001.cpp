#include <sge_sita_ray_tracer.h>

namespace sge {

class MainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
protected:
	virtual void onInitGL() override {
		Base::onInitGL();

		_debugRay.origin = MyVec3f::s_zero();
		_debugRay.dir = MyVec3f::s_zero();

//		_debugPlane.normal = MyVec3f(-1,-1,-1).normalize();
		_debugPlane.normal = MyVec3f::s_forward();
		_debugPlane.distance = -5;

//		_debugMesh.loadObjFile("Assets/models/sphere_smooth.obj");
		_debugSphere.center = {3,3,3};
		_debugSphere.radius = 1;

		_debugTri.v0 = { 1,1,1 };
		_debugTri.v1 = { 2,2,1 };
		_debugTri.v2 = { 1,2,0 };

		//_debugMesh.loadObjFile("Assets/models/test.obj");
		_debugMesh.loadObjFile("Assets/models/test2.obj");
		_debugMesh.wireframe = true;
	}

	virtual void onDraw() override {
		setFrameBufferSize(_clientRect.size);
		
		my_drawGrid();
		my_drawOriginAxis();

		glPointSize(10);

		_debugMesh.draw();
		_debugRay.draw();
		_debugPlane.draw();
		_debugSphere.draw();
		_debugTri.draw();
		_hitResult.draw();

		swapBuffers();
		drawNeeded();
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
				} break;
			}
		}
	}

	void rayTracing(float x, float y) {
		glGetFloatv(GL_PROJECTION_MATRIX, _tmpProjMatrix.data);
		glGetFloatv(GL_MODELVIEW_MATRIX, _tmpModelview.data);

		MyVec2f screenSize = _frameBufferSize;

		_rayTraser.init(screenSize, _tmpProjMatrix, _tmpModelview);

		MyRay3f ray = _rayTraser.getRay(x, y);

		_debugRay = ray;

		_hitResult.reset();
		ray.raycast(_hitResult, _debugPlane, _hitResult.distance);
		ray.raycast(_hitResult, _debugSphere, _hitResult.distance);
		ray.raycast(_hitResult, _debugTri, _hitResult.distance);
		ray.raycast(_hitResult, _debugMesh, _hitResult.distance);
	}

private:
	MyRay3f				_debugRay;

	MyRay3f::HitResult	_hitResult;
	MyRayTracer			_rayTraser;

	Math::MyPlane3f		_debugPlane;
	Math::MySphere3f	_debugSphere;
	Math::MyTriangle3f	_debugTri;
	MyMesh				_debugMesh;

	MyMat4f				_tmpProjMatrix;
	MyMat4f				_tmpModelview;
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
