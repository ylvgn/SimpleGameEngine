#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_Chap04_Common.h"

namespace sge {

class PW5_MySysMets2 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MySysMets2;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
private:

	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);
	static WNDPROC s_defaultWndProc;

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	void _onScrollV(int y);

	int  _cxChar;
	int  _cyChar;
	int  _cxCaps;
	int  _viewportH;
	int  _scrollPosV;
};

}

#endif // SGE_OS_WINDOWS