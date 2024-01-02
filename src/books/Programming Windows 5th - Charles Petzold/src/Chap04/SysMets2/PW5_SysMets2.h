#pragma once

#include "../PW5_Chap04_Common.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_SysMets2 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_SysMets2;
protected:
	virtual void onCreate(CreateDesc& desc) override;
private:
	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);
};

}

#endif // SGE_OS_WINDOWS