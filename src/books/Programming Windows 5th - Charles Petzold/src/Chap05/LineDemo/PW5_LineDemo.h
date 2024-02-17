#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_LineDemo : public PW5_NativeUIWindow {
	using This = PW5_LineDemo;
	using Base = PW5_NativeUIWindow;
protected:
	virtual void onDraw() override;
private:
	void _example1();
};

}

#endif // SGE_OS_WINDOWS