#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* rotation
	* FYI: https://nehe.gamedev.net/tutorial/rotation/14001/
*/

class NeHeOGL_Lesson004 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson004;
protected:
	virtual void onDraw() override;
private:
	void _example1(float upTime);
	void _example1_rotateTheory(float upTime);
	void _example1_rotateInOpenGL(float upTime);

	void _example2(float upTime);

	MyHiResTimer _uptime;
};

}