#pragma once

#include "../MyCommon.h"

namespace sge {

class MyOpenGLNativeUIWindow_Base : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	void beginRender()	{ onBeginRender(); }
	void endRender()	{ onEndRender(); }
	void swapBuffers()	{ onSwapBuffers(); }

	void setFrameBufferSize(const Vec2f& newSize);

	Vec2f			_frameBufferSize {0};

	Math::Camera3f	_camera;
	Mat4f			_matrix_proj;
	Mat4f			_matrix_view;
	Mat4f			_matrix_vp;

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
	virtual void onCloseButton() override;

	virtual void onInitGL()			{}
	virtual void onSwapBuffers()	{}
	virtual void onBeginRender()	{}
	virtual void onEndRender()		{}
};

}