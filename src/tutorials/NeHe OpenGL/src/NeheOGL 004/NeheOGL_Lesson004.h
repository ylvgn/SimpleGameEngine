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
	void _example1(float upTime);
	void _example1_rotateTheory(float upTime);
	void _example1_rotateInOpenGL(float upTime);

	void _example2(float upTime);

	MyHiResTimer _uptime;
};

}