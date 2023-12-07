#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_SysMets1 : public ProgWin5WindowBase {
	using Base = ProgWin5WindowBase;
	using This = PW5_SysMets1;
protected:
	virtual WndProcHandler onGetWndProc() { return &s_WndProc; }
private:
	static LRESULT CALLBACK s_WndProc(HWND, UINT, WPARAM, LPARAM);
};

}

#endif // SGE_OS_WINDOWS