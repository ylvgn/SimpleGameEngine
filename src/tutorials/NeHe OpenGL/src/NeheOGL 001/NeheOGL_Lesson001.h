#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* create an empty opengl window
	* FYI: https://nehe.gamedev.net/tutorial/creating_an_opengl_window_(win32)/13001/
*/
class NeheOGL_Lesson001 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson001;
protected:
	virtual void onDraw() override;
};

}