#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* draw triangles and quads
	* FYI: https://nehe.gamedev.net/tutorial/your_first_polygon/13002/
*/

class NeHeOGL_Lesson002 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson002;
protected:
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();
	void _example3();
};

}