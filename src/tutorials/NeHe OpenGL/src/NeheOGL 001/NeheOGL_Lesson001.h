#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* create an empty opengl window
	* FYI: https://nehe.gamedev.net/tutorial/creating_an_opengl_window_(win32)/13001/
*/
class NeHeOGL_Lesson001 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson001;
protected:
	virtual void onDraw() override;
};

}