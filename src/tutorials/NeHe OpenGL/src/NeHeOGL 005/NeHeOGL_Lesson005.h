#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* 3D Shapes: cone and cube
	* FYI: https://nehe.gamedev.net/tutorial/3d_shapes/10035/
*/

class NeHeOGL_Lesson005 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson005;
protected:
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onDraw() override;
private:

	void _example1();
	void _shaded(float angleInDegrees);
	void _wireFrame(float angleInDegrees);

	void _example2();
	void _cullFace();
	void _frontFace();
	void _depthTest();
	void _bufferWriteMask();
	void _zFight();

	void _example3();
	void _example4();

	static constexpr float d = 1.f;

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
		{-d, d,-d}, // 0
		{ d, d,-d}, // 1
		{ d, d, d}, // 2
		{-d, d, d}, // 3
		{-d,-d,-d}, // 4
		{ d,-d,-d}, // 5
		{ d,-d, d}, // 6
		{-d,-d, d}, // 7
	};

	MyHiResTimer _uptime;

	float _cameraX = 30;
	float _cameraY = 30;
};

}