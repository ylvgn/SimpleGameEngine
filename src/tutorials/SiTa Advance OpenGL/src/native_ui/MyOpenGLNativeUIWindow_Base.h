#pragma once

#include "../MyCommon.h"

namespace sge {

class MyOpenGLNativeUIWindow_Base : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	void destroy()		{ onDestroy(); } // is it allright ???
	void swapBuffers()	{ onSwapBuffers(); }

	void drawGrid();
	void drawOriginAxis();

	void beginRender() { onBeginRender(); }
	void endRender() { onEndRender(); }

	void setFrameBufferSize(const Vec2f& newSize);

	Vec2f	_frameBufferSize{ 0,0 };

	Math::Camera3f	_camera;
	Mat4f			_matrix_proj;
	Mat4f			_matrix_view;
	Mat4f			_matrix_vp;

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;

	virtual void onCloseButton() {
		destroy();
		NativeUIApp::current()->quit(0);
	}

	virtual void onInitGL() {}
	virtual void onDestroy() {}
	virtual void onSwapBuffers() {}

	virtual void onBeginRender() {}
	virtual void onEndRender() {}
};

}