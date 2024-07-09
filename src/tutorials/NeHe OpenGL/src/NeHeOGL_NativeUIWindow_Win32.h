#pragma once

#if SGE_OS_WINDOWS

#include "NeHeOGL_GL_Common.h"

namespace sge {

class NeHeOGL_NativeUIWindow_Win32 : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	~NeHeOGL_NativeUIWindow_Win32() {
		_destroy();
	}

	void initGL();
	void clearGL();

	void makeCurrent();
	void swapBuffers();

	void setFrameBufferSize(const Vec2f& newSize);

protected:
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onCloseButton() override;

	virtual void onInitedGL() {};

	void _destroy();

	Math::Camera3f	_camera;
	Mat4f			_matrix_view;
	Mat4f			_matrix_proj;

	Vec2f _frameBufferSize;

	HDC		_dc = nullptr; // GDI Device Context: The DC connects the Window to the GDI
	HGLRC	_rc = nullptr; // OpenGL Rendering Context: The RC connects OpenGL to the DC
};

}

#endif