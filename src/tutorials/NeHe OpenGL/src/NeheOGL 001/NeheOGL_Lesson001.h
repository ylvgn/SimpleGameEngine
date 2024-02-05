#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {

class NeheOGL_Lesson001 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson001;
public:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onDraw() override;
private:
	void _resizeGLScene();
	void _drawGLScene();
};

}