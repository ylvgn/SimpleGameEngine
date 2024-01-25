#pragma once
#if SGE_OS_WINDOWS

#include "../PW5_Chap04_Common.h"

namespace sge {

class PW5_SysMets1 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_SysMets1;
protected:
	virtual void onCreate(CreateDesc& desc) override;
private:
	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);
};

}

#endif // SGE_OS_WINDOWS