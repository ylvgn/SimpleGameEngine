#include "NeHeOGL_Lesson009_NativeUIWindow.h"

namespace sge {

void NeHeOGL_Lesson009_NativeUIWindow::onInitedGL() {
	_gridMesh.createGrid(10);
	_gridMesh.renderState.wireframe = true;

	_coordinateMesh.createCoordinate();

	OGLUtil::throwIfError();
}

void NeHeOGL_Lesson009_NativeUIWindow::onUIMouseEvent(UIMouseEvent& ev) {
	using Button	= UIMouseEvent::Button;
	using Type		= UIMouseEvent::Type;
	using Modifier	= UIEventModifier;

	if (ev.isDragging()) {
		switch (ev.pressedButtons) {
			case Button::Left: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					float d = 0.15f;
					_camerOrbitAngle += ev.deltaPos.yx() * d;
				}
			} break;
			case Button::Right: {
				float d = 0.15f;
				_camerOrbitAngle += ev.deltaPos.yx() * d;
			} break;
			case Button::Middle: {
				float d = 0.01f;
				_camerMovePos.x += ev.deltaPos.x * d;
				_camerMovePos.y += ev.deltaPos.y * -d;
			} break;
		}
	}

	if (ev.isScroll()) {
		auto d = ev.scroll * 0.005f;
		_camerMovePos.z -= d.y;
	}
}

void NeHeOGL_Lesson009_NativeUIWindow::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	if (ev.isDown(KeyCode::PageUp)) _camerMovePos.z += 0.02f;
	if (ev.isDown(KeyCode::PageDown)) _camerMovePos.z -= 0.02f;

	if (ev.isDown(KeyCode::F)) {
		SGE_DUMP_VAR(_camerOrbitAngle, _camerMovePos);
	}
}

void NeHeOGL_Lesson009_NativeUIWindow::beginOpaquePass() {
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void NeHeOGL_Lesson009_NativeUIWindow::endOpaquePass() {

}

void NeHeOGL_Lesson009_NativeUIWindow::beginTransparentPass() {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void NeHeOGL_Lesson009_NativeUIWindow::endTransparentPass() {
	glDepthMask(GL_TRUE);
}

void NeHeOGL_Lesson009_NativeUIWindow::_setViewport() {
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));
}

void NeHeOGL_Lesson009_NativeUIWindow::_setDefaultRenderState() {
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_TEXTURE_2D);

	glColor4f(1, 1, 1, 1);
	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
	glClearDepth(1);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
}

void NeHeOGL_Lesson009_NativeUIWindow::_initCamera() {
	float width  = _clientRect.w;
	float height = _clientRect.h;

	if (height == 0) height = 1;

	float aspect = width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, aspect, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -_camerMovePos.z);
	glRotatef(_camerOrbitAngle.x, 1,0,0);
	glRotatef(_camerOrbitAngle.y, 0,1,0);
}

void NeHeOGL_Lesson009_NativeUIWindow::_beginRender() {
	_setViewport();
	_setDefaultRenderState();
	_initCamera();

//----
	OGL::drawGridAndCoordinate(_gridMesh, _coordinateMesh);
}

void NeHeOGL_Lesson009_NativeUIWindow::_endRender() {
	swapBuffers();
	drawNeeded();
}

void NeHeOGL_Lesson009_NativeUIWindow::onDraw() {
	_beginRender();
		onRender();
	_endRender();
}

}