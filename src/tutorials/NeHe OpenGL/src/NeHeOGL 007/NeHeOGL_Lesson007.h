#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* Texture Filters, Lighting
	* FYI: https://nehe.gamedev.net/tutorial/texture_filters,_lighting_&_keyboard_control/15002/
*/

#define NeHeOGL_Lesson007_SphereMode_ENUM_LIST(E) \
	E(Flat,		= 0) \
	E(Smooth,	= 1) \
	E(High,		= 2) \
//-------
SGE_ENUM_DECLARE(NeHeOGL_Lesson007_SphereMode, int)

#define NeHeOGL_Lesson007_LightType_ENUM_LIST(E) \
	E(Directional,	= 0) \
	E(Point,		= 1) \
	E(Spot,			= 2) \
//-------
SGE_ENUM_DECLARE(NeHeOGL_Lesson007_LightType, int)

class NeHeOGL_Lesson007 : public NeHeOGL_NativeUIWindow {
	using Base			= NeHeOGL_NativeUIWindow;
	using This			= NeHeOGL_Lesson007;
	using SphereMode	= NeHeOGL_Lesson007_SphereMode;
	using LightType		= NeHeOGL_Lesson007_LightType;

	static const int kTexture2dCount = 3;

public:
	NeHeOGL_Lesson007()
		: _isFloorOnMipMap(true)
		, _isDrawOneSphere(false)
		, _isShowSphereNormals(false)
		, _isWireFrame(false)
		, _isOnAmbient(true)
		, _isOnDiffuse(true)
		, _isOnSpecular(true)
		, _isPressedF(false)
		, _isPressedT(false)
		, _isPressedM(false)
		, _isPressedO(false)
		, _isPressedN(false)
		, _isPressedW(false)
		, _isPressedA(false)
		, _isPressedD(false)
		, _isPressedS(false)
	{}

protected:
	virtual void onInitedGL() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
	virtual void onDraw() override;
private:

	void _example1();
	void _example2();
	void _example3(LightType lightType);

	void _exampleDirectionalLight(float uptime);
	void _examplePointLight(float uptime);
	void _exampleSpotLight(float uptime);

	void _drawSpheres(SphereMode mode);
	void _drawSpheres(Mesh& mesh);
	void _drawSphereNormals(SphereMode mode);
	void _drawSphereNormals(const Mesh& mesh);

	void _drawLightPointGizmos();

	bool _isFloorOnMipMap		: 1;
	bool _isDrawOneSphere		: 1;
	bool _isShowSphereNormals	: 1;
	bool _isWireFrame			: 1;
	bool _isOnAmbient			: 1;
	bool _isOnDiffuse			: 1;
	bool _isOnSpecular			: 1;
	bool _isPressedF			: 1;
	bool _isPressedT			: 1;
	bool _isPressedM			: 1;
	bool _isPressedO			: 1;
	bool _isPressedN			: 1;
	bool _isPressedW			: 1;
	bool _isPressedA			: 1;
	bool _isPressedD			: 1;
	bool _isPressedS			: 1;

	Vec2f _camerOrbitSpeed { 0,0 };			// the speed the crate is spinning at on the x/y axis
	Vec2f _camerOrbitAngle { 30.f, -30.f };	// camera rotate with x-axis y-axis (degrees)
	float _camerMovePosZ = 5.f;

	Texture2D	_texture2dArray[kTexture2dCount]; // Storage For 3 Textures
	GLuint		_texture2ds[kTexture2dCount];

	GLuint	_texSelectedIndex = 0; // Which Texture Filter To Use
	Image	_imageToUpload;

	Vec4f	_globalAmbient	{ 0.2f, 0.2f, 0.2f, 1.f };

	// Light is created the same way color is created
	Vec4f	_lightAmbient	{ 0,0,0,1 };
	Vec4f	_lightDiffuse	{ 1,1,1,1 };
	Vec4f	_lightSpecular	{ 1,1,1,1 };
	Vec4f	_lightPos		{ 0,0,0,0 };

	Vec3f	_spotDir { 0,0,1 };
	float	_spotExponent = 0;
	float	_spotCutoffAngle = 60; // 0~90, 180

	Mesh	_gridMesh;
	Mesh	_coordinateMesh;

	Mesh		_floorMesh;
	Texture2D	_floorMainTex;
	Texture2D	_floorMainTexNoMipMap;

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

	Vec4f		_ambient	{ 0,0,1,1 };
	Vec4f		_diffuse	{ 0,1,0,1 };
	Vec4f		_specular	{ 1,0,0,1 };
	Vec4f		_emission	{ 0,0,0,0 };

	SphereMode	_sphereMode = SphereMode::Flat;

	Mesh		_sphereFlatMesh;
	Mesh		_sphereSmoothMesh;
	Mesh		_sphereHighMesh;

	float		_shininess = 64;

	Vector<Vec3f> _sphereNormals;

	MyHiResTimer  _uptime;
};

inline
const char* enumStr(NeHeOGL_Lesson007_SphereMode v) {
	using SRC = NeHeOGL_Lesson007_SphereMode;
	switch (v) {
#define E(T) case SRC::T: return #T;
		E(Flat)
		E(Smooth)
		E(High)
#undef E
	default: { SGE_ASSERT(false); return ""; }
	}
}

inline
const char* enumStr(NeHeOGL_Lesson007_LightType v) {
	using SRC = NeHeOGL_Lesson007_LightType;
	switch (v) {
#define E(T) case SRC::T: return #T;
		E(Directional)
		E(Point)
		E(Spot)
#undef E
	default: { SGE_ASSERT(false); return ""; }
	}
}

SGE_FORMATTER_ENUM(NeHeOGL_Lesson007_SphereMode)
SGE_FORMATTER_ENUM(NeHeOGL_Lesson007_LightType)
}