#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_MyDrawText : public ProgWin5WindowBase {
	using This = PW5_MyDrawText;
	using Base = ProgWin5WindowBase;
protected:
	virtual WNDPROC onGetWndProc() override { return This::s_WndProc; }
private:
	static LRESULT CALLBACK s_WndProc(HWND, UINT, WPARAM, LPARAM);
};

}

#endif // SGE_OS_WINDOWS