#include <sge_sita_ray_tracer.h>

namespace sge {

class MainWin : public MyOpenGLNativeUIWindow {
	using Base = MyOpenGLNativeUIWindow;
protected:
	virtual void onInitGL() override {
		Base::onInitGL();

		_mesh.loadObjFile("Assets/models/test.obj");
//		_mesh.loadObjFile("Assets/models/test2.obj");
		_mesh.wireframe = true;
	}

	void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);

		using Button = UIMouseEvent::Button;

		if (ev.isDown()) {
			switch (ev.pressedButtons) {
				case Button::Left: {
					rayTracing(ev.pos.x, ev.pos.y);
				}break;
				case Button::Middle: {
					_tex.destroy();
				}break;
				case Button::Right: {
					_tex.destroy();
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

		_mesh.draw();
		_mesh.aabb.draw();

//		testShowScreenSpace();
		drawTextureOnScreen();

		swapBuffers();
		drawNeeded();
	}

	void testShowScreenSpace() {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		Scoped_glDisable depthTest(GL_DEPTH_TEST);
		{
			Scoped_glBegin begin(GL_QUADS);
			float d = 0.5f;
			glColor4f(0.2f, 0, 0, 1);
			glVertex3f(-d, -d, 0);
			glVertex3f( d, -d, 0);
			glVertex3f( d,  d, 0);
			glVertex3f(-d,  d, 0);
		}

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	void drawTextureOnScreen() {
		if (!_tex.valid())
			return;

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		Scoped_glDisable depthTest(GL_DEPTH_TEST);
		{
			float cw = static_cast<float>(_rayTracer.canvasWidth());
			float ch = static_cast<float>(_rayTracer.canvasHeight());
			float u = cw / _tex.width();
			float v = ch / _tex.height();
			_tex.bind();
			{
				Scoped_glBegin begin(GL_QUADS);
				static float d = 1.f;
				// first uv, then vertex pos
				// in OpenGL y is reverse
				glTexCoord2f(0, v); glVertex3f(-d, -d, 0);
				glTexCoord2f(u, v); glVertex3f( d, -d, 0);
				glTexCoord2f(u, 0); glVertex3f( d,  d, 0);
				glTexCoord2f(0, 0); glVertex3f(-d,  d, 0);
			}
			_tex.unbind();

		}

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	void rayTracing(float x, float y) {
		glGetFloatv(GL_PROJECTION_MATRIX, _tmpProjMatrix.data);
		glGetFloatv(GL_MODELVIEW_MATRIX, _tmpModelview.data);

		_rayTracer.init(_frameBufferSize, _tmpProjMatrix, _tmpModelview);
		_rayTracer.render(_mesh);
		{
			int cw = _rayTracer.canvasWidth();
			int ch = _rayTracer.canvasHeight();

			int texWidth = static_cast<int>(Math::nextPow2(cw));
			int texHeight = static_cast<int>(Math::nextPow2(ch));

			SGE_DUMP_VAR(cw, ch, texWidth, texHeight);

			_tex.create(texWidth, texHeight, nullptr);
			_tex.subImage(0, 0, cw, ch, _rayTracer.pixels_ptr());
		}
	}

private:
	MyMat4f			_tmpProjMatrix;
	MyMat4f			_tmpModelview;

	MyRayTracerf	_rayTracer;
	MyMesh			_mesh;
	MyTexture2D		_tex;
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
			//winDesc.rect = { 10, 10, 720, 480 };
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
