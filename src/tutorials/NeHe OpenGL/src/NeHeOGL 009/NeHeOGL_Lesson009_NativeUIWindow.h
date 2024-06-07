#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {

class NeHeOGL_Lesson009_NativeUIWindow : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson009_NativeUIWindow;
protected:
	virtual void onInitedGL() override;
	virtual void onDraw() override final;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;

	virtual void onRender() {};

	void beginOpaquePass();
	void endOpaquePass();

	void beginTransparentPass();
	void endTransparentPass();

	void setCameraPos(const Vec3f& pos)			 { _camerMovePos.set(pos); }
	void setCameraOrbitAngle(const Vec2f& angle) { _camerOrbitAngle.set(angle); }
	void setClearColor(const Color4f& color)	 { _clearColor.set(color); }
private:
	void _beginRender();
		void _setViewport();
		void _setDefaultRenderState();
		void _initCamera();
	void _endRender();

	Mesh	_gridMesh;
	Mesh	_coordinateMesh;

	Vec2f	_camerOrbitAngle{ 30,30 };
	Vec3f	_camerMovePos{ 0,0,5 };

	Color4f _clearColor { 0.f, 0.2f, 0.2f, 0.f };
};

}