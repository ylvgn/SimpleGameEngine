#pragma once

#include "PW5_NativeUI.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_TextMetric : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_TextMetric;
protected:
	virtual void onPaint(ScopedPaintStruct& ps) override;
};

}

#endif // SGE_OS_WINDOWS