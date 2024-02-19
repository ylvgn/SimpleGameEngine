#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* draw triangles and quads
	* FYI: https://nehe.gamedev.net/tutorial/your_first_polygon/13002/
*/

class NeheOGL_Lesson002 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson002;
public:
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();
	void _example3();
};

}