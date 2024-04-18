#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_RandRect : public PW5_NativeUIWindow {
	using This		= PW5_RandRect;
	using Base		= PW5_NativeUIWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onUpdate(float dt) override;
private:
	Rect2f	_rc;
	Color4b _color;
};

}

#endif // SGE_OS_WINDOWS