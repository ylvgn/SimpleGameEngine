#pragma once

#if SGE_OS_WINDOWS

#include "../../Chap04/PW5_Chap04_Common.h"

namespace sge {

class PW5_DevCaps1 : public PW5_MyDefineMarkWindow {
	using This = PW5_DevCaps1;
	using Base = PW5_MyDefineMarkWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
};

}

#endif // SGE_OS_WINDOWS