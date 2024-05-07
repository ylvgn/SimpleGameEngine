#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* Blending
	* FYI: https://nehe.gamedev.net/tutorial/blending/16001/
*/

class NeHeOGL_Lesson008 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson008;
public:
	NeHeOGL_Lesson008()
		: _isBlend(true)
		, _isPressedB(false)
	{}

protected:
	virtual void onInitedGL() override;
	virtual void onDraw() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
private:

	void _example1();
	void _example2();
	void _example3();
	void _example4();
	void _example5();
	void _example6();

	void _beginRender();
		void _setViewport();
		void _setDefaultRenderState();
		void _initCamera();

		void _beginOpaquePass();
		void _endOpaquePass();
		void _beginTransparentPass();
		void _endTransparentPass();
	void _endRender();

	Vec2f _camerOrbitAngle	{30,30};	// camera rotate with x-axis y-axis
	Vec3f _camerMovePos		{0,0,5};	// camera move, zoom in/out

	Mesh		_rOpaque;
	Mesh		_gTransparent;
	Mesh		_bTransparent;

	Texture2D	_rTex;
	Texture2D	_gTex;
	Texture2D	_bTex;

	Mesh		_opaqueRect;
	Mesh		_transparentRect;
	Texture2D	_colorBleedingTex;
	Texture2D	_pmaTex;

	Mesh		_gridMesh;
	Mesh		_coordinateMesh;

	Mesh		_cubeMesh;
	Texture2D	_glassTex;

	bool	_isPressedB	: 1;
	bool	_isBlend	: 1; // Blending OFF / ON?

	Vec4f	_rotXY { 0,0,0,0 }; // { X Rotation, Y Rotation, X Rotation Speed Y Rotation Speed}
};

}