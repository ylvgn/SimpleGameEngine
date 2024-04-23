#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* Texture Filters, Lighting
	* FYI: https://nehe.gamedev.net/tutorial/texture_filters,_lighting_&_keyboard_control/15002/
*/

class NeHeOGL_Lesson007 : public NeHeOGL_NativeUIWindow {
	using Base		= NeHeOGL_NativeUIWindow;
	using This		= NeHeOGL_Lesson007;

	using Texture2D = NeHeOGL_Texture2D;
	using Filter	= Texture2D::SamplerState::Filter;

	static const int kTexture2dCount = 3;

public:
	NeHeOGL_Lesson007()
		: _isOnLight(true)
		, _isPressedL(false)
		, _isPressedF(false)
	{}

protected:
	virtual void onInitedGL() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
	virtual void onDraw() override;
private:

	void _example1();

	bool _isOnLight		: 1; // Lighting ON / OFF
	bool _isPressedL	: 1;
	bool _isPressedF	: 1;

	Vec2f _camerOrbitSpeed { 0,0 };			// the speed the crate is spinning at on the x/y axis
	Vec2f _camerOrbitAngle { 30.f, -30.f };	// camera rotate with x-axis y-axis (degrees)
	float _camerMovePosZ = -5.f;

	NeHeOGL_Texture2D _texture2dArray[kTexture2dCount]; // Storage For 3 Textures
	GLuint _texture2ds[kTexture2dCount];

	GLuint	_texSelectedIndex = 0; // Which Texture Filter To Use
	NeHeOGL_Image _imageToUpload;

	// Light is created the same way color is created
	Tuple4f _lightAmbient  { 0.5f, 0.5f, 0.5f, 1.0f };
	Tuple4f _lightDiffuse  { 1.0f, 1.0f, 1.0f, 1.0f };

	Vec4f	_lightPosition { 0.0f, 0.0f, 2.0f, 1.0f };

	NeHeOGL_Mesh _gridMesh;

//------------------------------------------
//		  0--------1
//		 /|       /|
//		3--------2 |
//      | |      | |
//		| 4------|-5
//      |/       |/
//      7--------6

	static constexpr float a = 0.5f;
	static const int kCubeVertexCount = 8;
	Vector<Tuple3f, kCubeVertexCount> kCubePos = {
		{-a, a,-a}, // 0
		{ a, a,-a}, // 1
		{ a, a, a}, // 2
		{-a, a, a}, // 3
		{-a,-a,-a}, // 4
		{ a,-a,-a}, // 5
		{ a,-a, a}, // 6
		{-a,-a, a}, // 7
	};
};

}