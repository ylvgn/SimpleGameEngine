#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* add vertex color
	* FYI: https://nehe.gamedev.net/tutorial/adding_colour/13003/
*/

class NeheOGL_Lesson003 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson003;
protected:
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();

	void _glW() const { glColor4f(1,1,1,1); } // white
	void _glR() const { glColor4f(1,0,0,1); } // red
	void _glG() const { glColor4f(0,1,0,1); } // green
	void _glB() const { glColor4f(0,0,1,1); } // blue
	void _glY() const { glColor4f(1,1,0,1); } // yellow
	void _glP() const { glColor4f(1,0,1,1); } // purple
	void _glC() const { glColor4f(0,1,1,1); } // cyan

	void _glColor(const Color4f& c) const { glColor4f(c.r, c.g, c.b, c.a); }
};

}