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
protected:
	virtual void onDraw() override;
private:
	void _example(float uptime, bool bWireframe);
	void _shaded(float angle);
	void _wireFrame(float angle);

	static constexpr float d = 1;

//------------------------------------------
//		   1-----2
//		  /  0  /
//		 3-----4
	static constexpr float kCone[5][3] = {
		{ 0,  d,  0}, // 0 Top
		{-d, -d, -d}, // 1
		{ d, -d, -d}, // 2
		{-d, -d,  d}, // 3
		{ d, -d,  d}, // 4
	};

//------------------------------------------
//		  0--------1
//		 /|       /|
//		3--------2 |
//      | |      | |
//		| 4------|-5
//      |/       |/
//      7--------6
	static constexpr float kCube[8][3] = {
		{-d,  d, -d}, // 0
		{ d,  d, -d}, // 1
		{ d,  d,  d}, // 2
		{-d,  d,  d}, // 3
		{-d, -d, -d}, // 4
		{ d, -d, -d}, // 5
		{ d, -d,  d}, // 6
		{-d, -d,  d}, // 7
	};

	MyHiResTimer _uptime;
};

}