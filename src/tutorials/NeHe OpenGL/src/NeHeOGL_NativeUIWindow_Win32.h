#pragma once

#if SGE_OS_WINDOWS

#include "NeHeOGL_Common.h"

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

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onCloseButton() override;

	virtual void onInitedGL() {};

private:
	void _destroy();

	HDC		_dc = nullptr; // GDI Device Context: The DC connects the Window to the GDI
	HGLRC	_rc = nullptr; // OpenGL Rendering Context: The RC connects OpenGL to the DC
};

}

#endif