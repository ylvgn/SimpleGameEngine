#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_Chap04_Common.h"

namespace sge {

class PW5_MySysMets3 : public PW5_MyDefineMarkInfoWindow {
	using Base = PW5_MyDefineMarkInfoWindow;
	using This = PW5_MySysMets3;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
};

}

#endif // SGE_OS_WINDOWS