#include "MyOpenGLNativeUIWindow_Base.h"

namespace sge {

void MyOpenGLNativeUIWindow_Base::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	_camera.setPos(0, 10, 10);
	_camera.setAim(0, 0, 0);
}

void MyOpenGLNativeUIWindow_Base::onUIMouseEvent(UIMouseEvent& ev) {
	using Button	= UIMouseEvent::Button;
	using Type		= UIMouseEvent::Type;
	using Modifier	= UIEventModifier;

	if (ev.isDragging()) {
		switch (ev.pressedButtons) {
			case Button::Left: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					auto d = ev.deltaPos * -0.005f;
					_camera.orbit(d.x, d.y);
				}
			}break;
			case Button::Middle: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					auto d = ev.deltaPos * 0.05f;
					_camera.move(d.x, d.y, 0);
				}
			}break;
			case Button::Right: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
					auto d = ev.deltaPos * 0.03f;
					_camera.dolly(d.y);
				} else {
					auto d = ev.deltaPos * -0.005f;
					_camera.orbit(d.x, d.y);
				}
			}break;
		}
	}

	if (ev.isScroll()) {
		auto d = ev.scroll * -0.005f;
		_camera.dolly(d.y);
	}
}

void MyOpenGLNativeUIWindow_Base::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	if (ev.isDown(KeyCode::Ctrl) && ev.isDown(KeyCode::W)) {
		onCloseButton();
	}
	if (ev.isDown(KeyCode::F)) {
		SGE_DUMP_VAR(_camera.pos(), _camera.aim());
	}
}

void MyOpenGLNativeUIWindow_Base::setFrameBufferSize(const MyVec2f& newSize) {
	_camera.setViewport(_clientRect);
	_matrix_proj = _camera.projMatrix();
	_matrix_view = _camera.viewMatrix();

	_frameBufferSize = newSize;
	glViewport(0, 0, static_cast<int>(newSize.x), static_cast<int>(newSize.y));

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glPointSize(1);
	glLineWidth(1);
	glColor4f(1, 1, 1, 1);

	glClearColor(0.0f, 0.2f, 0.2f, 0);
	glClearDepth(1);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(_matrix_proj._columns->data);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(_matrix_view._columns->data);
}

}
