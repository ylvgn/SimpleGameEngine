#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_MyRectangle : public PW5_NativeUIWindow {
	using This		= PW5_MyRectangle;
	using Base		= PW5_NativeUIWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
private:
	void _example1();
	void _example2();
	void _example3();

	Vec2f _rectPos;
};

}

#endif // SGE_OS_WINDOWS