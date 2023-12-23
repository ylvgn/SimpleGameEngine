#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_TextMetric : public ProgWin5WindowBase {
	using This = PW5_TextMetric;
	using Base = ProgWin5WindowBase;

	static LRESULT CALLBACK s_WndProc(HWND, UINT, WPARAM, LPARAM);

protected:
	virtual WNDPROC onGetWndProc() override { return s_WndProc; }
};

}

#endif // SGE_OS_WINDOWS