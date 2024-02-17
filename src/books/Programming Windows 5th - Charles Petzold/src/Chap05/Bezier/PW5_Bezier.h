#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_Bezier : public PW5_NativeUIWindow {
	using This = PW5_Bezier;
	using Base = PW5_NativeUIWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
private:
	void _example1();

	Vector<::POINT, 4> _points;
};

}

#endif // SGE_OS_WINDOWS