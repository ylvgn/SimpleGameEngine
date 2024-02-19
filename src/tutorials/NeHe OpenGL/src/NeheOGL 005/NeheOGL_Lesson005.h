#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* 3D Shapes: cone and cube
	* FYI: https://nehe.gamedev.net/tutorial/3d_shapes/10035/
*/

class NeheOGL_Lesson005 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson005;
public:
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();

	float _rtri  = 0;
	float _rquad = 0;
};

}