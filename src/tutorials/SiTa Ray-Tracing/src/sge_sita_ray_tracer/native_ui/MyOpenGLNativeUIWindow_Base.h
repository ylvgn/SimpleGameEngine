#pragma once

#include "../MyCommon.h"

namespace sge {

class MyOpenGLNativeUIWindow_Base : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	void initGL() { onInitGL(); }
	void destroyGL() { onDestroyGL(); } // is it allright ???
	void swapBuffers() { onSwapBuffers(); }

	void drawGrid();
	void drawOriginAxis();

	void setFrameBufferSize(const Vec2f& newSize);

	Math::Camera3f	_camera;

	Mat4f	_matrix_proj;
	Mat4f	_matrix_view;
	Vec2f	_frameBufferSize{ 0, 0 };

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;

	virtual void onCloseButton() {
		destroyGL();
		NativeUIApp::current()->quit(0);
	}

	virtual void onInitGL() {}
	virtual void onDestroyGL() {}
	virtual void onSwapBuffers() {}
};

}