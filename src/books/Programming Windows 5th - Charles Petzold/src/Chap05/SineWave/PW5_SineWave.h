#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_SineWave : public PW5_NativeUIWindow {
	using This = PW5_SineWave;
	using Base = PW5_NativeUIWindow;
protected:
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();
	void _example3();

	static int const kNum = 1000;
	::POINT points[kNum];
};

}

#endif // SGE_OS_WINDOWS