#pragma once

#if SGE_OS_WINDOWS

#include <sge_core/native_ui/NativeUI.h>

namespace sge {

class NeheOGL_NativeUIWindow_Win32 : public NativeUIWindow {
	using Base = NativeUIWindow;
	using This = NeheOGL_NativeUIWindow_Win32;
public:
	~NeheOGL_NativeUIWindow_Win32() {
		destroy();
	}

	void destroy();

	void makeCurrent();
	void swapBuffers();

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onCloseButton() override;

private:
	void _initGL();

	HDC		_dc = nullptr; // GDI Device Context: The DC connects the Window to the GDI
	HGLRC	_rc = nullptr; // OpenGL Rendering Context: The RC connects OpenGL to the DC
};

}

#endif