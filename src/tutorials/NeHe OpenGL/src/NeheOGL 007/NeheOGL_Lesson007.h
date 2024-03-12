#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* Texture Filters, Lighting
	* FYI: https://nehe.gamedev.net/tutorial/texture_filters,_lighting_&_keyboard_control/15002/
*/

class NeheOGL_Lesson007 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson007;

	static const int kTexture2dCount = 3;

public:
	NeheOGL_Lesson007() : Base()
		, _isOnLight(true)
	{}

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
private:

	void _example1(float uptime);

	MyHiResTimer _uptime;

	bool _isOnLight  : 1;	// Lighting ON / OFF

	Vec2f _camerOrbitSpeed{ 0,0 };		// the speed the crate is spinning at on the x/y axis
	Vec2f _camerOrbitAngle{ 0,0 };		// camera rotate with x-axis y-axis
	float _camerMovePosZ;

	GLuint  _texture2ds[kTexture2dCount]; // Storage For 3 Textures
	GLuint	_texSelectedIndex = 0;		  // Which Texture Filter To Use
	MyImage _imageToUpload;

	// Light is created the same way color is created
	Tuple4f _lightAmbient  { 0.5f, 0.5f, 0.5f, 1.f };
	Tuple4f _lightDiffuse  { 1.f,  1.f,  1.f,  1.f };
	Tuple4f _lightPosition { 0.0f, 0.0f, 2.0f, 1.0f };
};

}