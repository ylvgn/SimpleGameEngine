#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_Chap04_Common.h"

namespace sge {

class PW5_MySysMets3 : public PW5_MyDefineMarkWindow {
	using Base = PW5_MyDefineMarkWindow;
	using This = PW5_MySysMets3;
protected:
	virtual void onCreate(CreateDesc& desc) override;
};

}

#endif // SGE_OS_WINDOWS