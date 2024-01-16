#pragma once

#include "../../PW5_NativeUI.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_MyTextMetrics : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MyTextMetrics;
protected:
	virtual void onDraw() override;
};

}

#endif // SGE_OS_WINDOWS