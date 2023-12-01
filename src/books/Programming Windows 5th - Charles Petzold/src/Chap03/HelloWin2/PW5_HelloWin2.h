#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_HelloWin2 : public ProgWin5WindowBase {
	using This = PW5_HelloWin2;
	using Base = ProgWin5WindowBase;
protected:
	virtual WndProcHandler onGetWndProc() override { return &s_WndProc; }
private:
	static LRESULT CALLBACK s_WndProc(HWND, UINT, WPARAM, LPARAM);
};

#endif

}