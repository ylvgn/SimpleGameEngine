#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* rotation
	* FYI: https://nehe.gamedev.net/tutorial/rotation/14001/
*/

class NeheOGL_Lesson004 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson004;
public:
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();

	void _example1_rotateTheory();
	void _example1_rotateInOpenGL();

	float _angleInDegree = 0;

	float _rtri  = 0;	// Angle For The Triangle
	float _rquad = 0;	// Angle For The Quad
};

}