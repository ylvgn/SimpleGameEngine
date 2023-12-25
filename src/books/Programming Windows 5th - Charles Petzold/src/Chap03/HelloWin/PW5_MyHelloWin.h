#pragma once

#include "PW5_NativeUI.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_MyHelloWin : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MyHelloWin;
protected:
	virtual void onCreate(CreateDesc& desc) override;
private:
	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);
};

}

#endif // SGE_OS_WINDOWS