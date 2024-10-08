#pragma once

#include "../MyCommon.h"

#include <sge_sita_ray_tracer/math/MyMat4.h>

namespace sge {

class MyOpenGLNativeUIWindow_Base : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	void initGL() { onInitGL(); }
	void swapBuffers() { onSwapBuffers(); }

	void setFrameBufferSize(const MyVec2f& newSize);

	Math::Camera3f	_camera;

	MyMat4f	_matrix_proj;
	MyMat4f	_matrix_view;
	MyVec2f	_frameBufferSize { 0,0 };

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
	virtual void onCloseButton() override;

	virtual void onInitGL() {}
	virtual void onSwapBuffers() {}
};

}