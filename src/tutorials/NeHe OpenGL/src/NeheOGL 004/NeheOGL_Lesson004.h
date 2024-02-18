#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* Rotation
	* FYI: https://nehe.gamedev.net/tutorial/rotation/14001/
*/

class NeheOGL_Lesson004 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson004;
public:
	virtual void onDraw() override;
private:
	void _example1();

	float _rtri  = 0;	// Angle For The Triangle
	float _rquad = 0;	// Angle For The Quad
};

}