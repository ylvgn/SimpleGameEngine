#pragma once

#include "NeHeOGL_Lesson009_NativeUIWindow.h"

namespace sge {
/*
	* Moving Bitmaps In 3D Space
	* FYI: https://nehe.gamedev.net/tutorial/moving_bitmaps_in_3d_space/17001/
*/

class NeHeOGL_Lesson009 : public NeHeOGL_Lesson009_NativeUIWindow {
	using Base = NeHeOGL_Lesson009_NativeUIWindow;
	using This = NeHeOGL_Lesson009;
public:
	static const int kStarCount = 50;
	static constexpr float kMaxDistFromOrigin = 5.f;

	NeHeOGL_Lesson009()
		: _isTwinkle(true)
		, _isPressedT(false)
	{}

protected:
	virtual void onInitedGL() override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
	virtual void onRender() override;
private:

	void _example1();

	struct StarInfo {
		Color4b color = OGL::kbWhite;
		float	distToOrigin = 0;
		float	angle = 0;
		Mesh	mesh;

		void randomColor();
		void setVertexColor(const Color4b& c);
	};

	Texture2D _starTex;
	Vector<StarInfo, kStarCount> _starList;

	bool _isTwinkle  : 1;
	bool _isPressedT : 1;

	float _tilt = 90.f;		// Tilt The View
	float _spin = 0.01f;	// Spin Twinkling Stars
};

}