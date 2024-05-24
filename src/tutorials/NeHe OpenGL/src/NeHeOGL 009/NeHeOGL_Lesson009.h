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
		Mesh mesh;

		Color4b color = OGL::kbWhite;

		float dist  = 0;
		float angle = 0;

		void randomColor();
	};

	Vector<StarInfo, kStarCount> _starList;
	Texture2D _starTex;

	bool _isTwinkle  : 1;
	bool _isPressedT : 1;

	float _zoom = -15;
	float _tilt = 90.f; // Tilt The View
	float _spin = 0.01f;// Spin Twinkling Stars

	int _loop;
};

}