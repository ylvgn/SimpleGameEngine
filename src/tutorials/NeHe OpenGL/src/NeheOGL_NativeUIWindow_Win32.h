#pragma once

#include "NeheOGL_Common.h"

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
	HDC		_dc = nullptr; // GDI Device Context: The DC connects the Window to the GDI
	HGLRC	_rc = nullptr; // OpenGL Rendering Context: The RC connects OpenGL to the DC
};

}