#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_HelloMsg2 : public ProgWin5WindowBase {
	using This = PW5_HelloMsg2;
	using Base = ProgWin5WindowBase;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual WndProcHandler onGetWndProc() { return &s_WndProcDefault; }
};

}

#endif // SGE_OS_WINDOWS