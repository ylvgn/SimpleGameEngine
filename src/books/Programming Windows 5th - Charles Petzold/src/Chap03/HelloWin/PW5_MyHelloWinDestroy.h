#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_MyHelloWinDestroy : public PW5_NativeUIWindow {
	using This = PW5_MyHelloWinDestroy;
	using Base = PW5_NativeUIWindow;
public:
	HWND _hwnd;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onCloseButton() override;
private:
	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
};

}

#endif // SGE_OS_WINDOWS