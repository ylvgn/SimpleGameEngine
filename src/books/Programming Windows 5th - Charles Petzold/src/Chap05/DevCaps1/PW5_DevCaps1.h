#pragma once

#if SGE_OS_WINDOWS

#include "../../Chap04/PW5_Chap04_Common.h"

namespace sge {

class PW5_DevCaps1 : public PW5_MyDefineMarkInfoWindow {
	using This = PW5_DevCaps1;
	using Base = PW5_MyDefineMarkInfoWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
};

}

#endif // SGE_OS_WINDOWS