#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {
/*
	-MessageBox
*/

class PW5_HelloMsg : public ProgWin5WindowBase {
	using This = PW5_HelloMsg;
	using Base = ProgWin5WindowBase;
protected:
	virtual void onCreate(CreateDesc& desc) override;
};

}

#endif // SGE_OS_WINDOWS