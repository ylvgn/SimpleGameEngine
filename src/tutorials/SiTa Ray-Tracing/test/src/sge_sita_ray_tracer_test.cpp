#include <sge_sita_ray_tracer.h>

namespace sge {

class Scoped_glPushMatrix : public NonCopyable {
public:
	Scoped_glPushMatrix() { glPushMatrix(); }
	~Scoped_glPushMatrix() { glPopMatrix(); }
};

class Scoped_glBegin : public NonCopyable {
public:
	Scoped_glBegin(GLenum mode) { glBegin(mode); }
	~Scoped_glBegin() { glEnd(); }
};

class TestRayCastWindow : public MyOpenGLNativeUIWindow {
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

		_debugMesh.loadObjFile("Assets/models/test.obj");
	}

	virtual void onDraw() override {
		setFrameBufferSize(_clientRect.size);
		
		drawGrid();
		drawOriginAxis();

		glPointSize(10);

		_debugMesh.wireframe = true;
		_debugMesh.draw();

		drawDebugRay();
		drawDebugPlane();
		drawDebugSphere();
		drawDebugTriangle();
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

		_hitResult.reset();
		ray.raycast(_hitResult, _debugPlane, _hitResult.distance);
		ray.raycast(_hitResult, _debugSphere, _hitResult.distance);
		ray.raycast(_hitResult, _debugTri, _hitResult.distance);
		ray.raycast(_hitResult, _debugMesh, _hitResult.distance);
	}

	void drawDebugRay() {
		drawDebugRay(_debugRay);
	}

	void drawRaycastResult() {
		drawRaycastResult(_hitResult);
	}

	void drawDebugPlane() {
		drawDebugPlane(_debugPlane);
	}

	void drawDebugPlane(const Math::MyPlane3f& plane, float size = 5) {
		const auto& nl = plane.normal;
		MyVec3f pt = nl * plane.distance;

		MyVec3f a, b, c, d;
		MyVec3f tangent, binormal;
		float s = plane.normal.dot(MyVec3f::s_right());
		if (Math::equals(Math::abs(s), 1.f)) {
			tangent = plane.normal.cross(MyVec3f::s_up());
		} else {
			tangent = plane.normal.cross(MyVec3f::s_right());
		}
		tangent = tangent.normalize();
		binormal = plane.normal.cross(tangent);

		a = pt + tangent * size;
		b = pt + binormal * size;
		c = pt - tangent * size;
		d = pt - binormal * size;

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(1, 1, 1, 1);
		{
			Scoped_glBegin begin(GL_LINES);
			glVertex3fv(pt.data);
			glVertex3fv((pt + nl).data);
		}

		{
			Scoped_glBegin begin(GL_QUADS);
			glVertex3fv(a.data);
			glVertex3fv(b.data);
			glVertex3fv(c.data);
			glVertex3fv(d.data);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void drawRaycastResult(const MyRay3f::HitResult& result, const Color4f& c = { 1,0,0,1 }, float lineWidth = 1) {
		if (!result.hasResult)
			return;

		glLineWidth(lineWidth);

		auto pt = result.point;
		auto nl = result.normal;

		{
			Scoped_glBegin begin(GL_POINTS);
			glColor4f(c.r, c.g, c.g, c.a);
				glVertex3fv(pt.data);
			glColor4f(1, 1, 1, 1);
		}

		{
			Scoped_glBegin begin(GL_LINES);
			glVertex3fv(pt.data);
			glVertex3fv((pt + nl).data);
		}

		glLineWidth(1);
	}

	void drawDebugTriangle() {
		drawDebugTriangle(_debugTri);
	}

	void drawDebugTriangle(const Math::MyTriangle3f& tri) {
		glColor4f(1,1,1,1);
		glLineWidth(1);

		{
			Scoped_glBegin begin(GL_LINE_LOOP);
			glVertex3fv(tri.v0.data);
			glVertex3fv(tri.v1.data);
			glVertex3fv(tri.v2.data);
		}

		{
			Scoped_glBegin begin(GL_LINES);
			auto c = tri.calcCenter();
			auto nl = tri.calcNormal();
			glVertex3fv(c.data);
			glVertex3fv((c+nl).data);
		}
	}

	void drawDebugRay(MyRay3f ray, const Color4f& c = {1,0,1,1}, float lineWidth = 1) {
		if (!ray.dir.sqrMagnitude())
			return;

		glLineWidth(lineWidth);
		auto start = ray.origin;
		auto end = ray.origin + ray.dir;

		{
			glColor4f(c.r, c.g, c.g, c.a);
			Scoped_glBegin begin(GL_POINTS);
			glVertex3fv(start.data);
			glColor4f(1,1,1,1);
		}

		{
			Scoped_glBegin begin(GL_LINES);
			glVertex3fv(start.data);
			glVertex3fv(end.data);
		}

		glLineWidth(1);
	}

	void drawDebugSphere() {
		drawDebugSphere(_debugSphere);
	}

	void drawDebugSphere(Math::MySphere3f& sphere, int subAxis = 16, int subHeight = 16) {
		if (subAxis < 2 || subHeight < 2)
			return;

		static float PI		= Math::PI<float>();
		static float PI2	= Math::PI<float>() * 2.f;

		const auto& c = sphere.center;
		float radius = sphere.radius;

		if (radius <= 0)
			return;

		Scoped_glPushMatrix m;
		glTranslatef(c.x, c.y, c.z);

		MyVec3f top    { 0,  radius, 0 };
		MyVec3f bottom { 0, -radius, 0 };

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_CULL_FACE);
		glColor4f(1,1,1,1);

		{ // top
			Scoped_glBegin begin(GL_TRIANGLES);
			int y = 1;
			float ty0 = PI * y / subHeight;
			float r0, h0;
			Math::sincos(ty0, r0, h0);

			for (int a = 0; a < subAxis; ++a) {
				float th0 = PI2 * a / subAxis;
				float th1 = PI2 * (a + 1) / subAxis;

				MyVec3f p0, p1;
				p0.y = p1.y = 0;
				Math::sincos(th0, p0.x, p0.z);
				Math::sincos(th1, p1.x, p1.z);

				MyVec3f v0 = p0 * radius * r0;
				MyVec3f v1 = p1 * radius * r0;
				v0.y = v1.y = h0;

				glVertex3fv(top.data);
				glVertex3fv(v1.data);
				glVertex3fv(v0.data);
			}
		}

		{ // middle
			Scoped_glBegin begin(GL_TRIANGLES);
			for (int y = 1; y < subHeight - 1; y++) {

				float ty0 = PI * y / subHeight;
				float ty1 = PI * (y+1) / subHeight;

				float r0 = Math::sin(ty0);
				float r1 = Math::sin(ty1);

				for (int a = 0; a < subAxis; ++a) {
					float th0 = PI2 * a / subAxis;
					float th1 = PI2 * (a+1) / subAxis;

					MyVec3f p0, p1;
					p0.y = p1.y = 0;
					Math::sincos(th0, p0.x, p0.z);
					Math::sincos(th1, p1.x, p1.z);

					MyVec3f v0 = p0 * radius * r0;
					MyVec3f v1 = p1 * radius * r0;

					MyVec3f v2 = p0 * radius * r1;
					MyVec3f v3 = p1 * radius * r1;

					v0.y = v1.y = Math::cos(ty0);
					v2.y = v3.y = Math::cos(ty1);

					glVertex3fv(v0.data);
					glVertex3fv(v1.data);
					glVertex3fv(v3.data);

					glVertex3fv(v0.data);
					glVertex3fv(v3.data);
					glVertex3fv(v2.data);
				}
			}
		}

		{ // bottom
			Scoped_glBegin begin(GL_TRIANGLES);

			int y = subHeight - 2;
			float ty1 = PI * (y+1) / subHeight;
			float r1, h1;
			Math::sincos(ty1, r1, h1);

			for (int a = 0; a < subAxis; ++a) {
				float th0 = PI2 * a / subAxis;
				float th1 = PI2 * (a+1) / subAxis;

				MyVec3f p0, p1;
				p0.y = p1.y = 0;
				Math::sincos(th0, p0.x, p0.z);
				Math::sincos(th1, p1.x, p1.z);

				MyVec3f v0 = p0 * radius * r1;
				MyVec3f v1 = p1 * radius * r1;
				v0.y = v1.y = h1;

				glVertex3fv(bottom.data);
				glVertex3fv(v0.data);
				glVertex3fv(v1.data);
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
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
	Math::MyPlane3f		_debugPlane;
	Math::MySphere3f	_debugSphere;
	Math::MyTriangle3f	_debugTri;
	MyMesh				_debugMesh;

	MyRay3f				_debugRay;
	MyRay3f::HitResult	_hitResult;
	MyRayTracer			_rayTraser;
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
