#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {

class NeheOGL_Lesson002 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson002;
public:
	virtual void onDraw() override;
};

}