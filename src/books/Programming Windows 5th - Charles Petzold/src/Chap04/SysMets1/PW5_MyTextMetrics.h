#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_MyTextMetrics : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MyTextMetrics;
protected:
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();

	MyTextMetrics _tm;
};

}

#endif // SGE_OS_WINDOWS