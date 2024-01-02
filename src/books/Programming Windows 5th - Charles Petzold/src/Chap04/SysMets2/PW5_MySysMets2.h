#pragma once

#include "../PW5_Chap04_Common.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_MySysMets2 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MySysMets2;
protected:
	virtual void onPostCreate() override;
	virtual void onOpen() override;
	virtual void onPaint(ScopedPaintStruct& ps) override;
private:

	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);
	static WNDPROC s_defaultWndProc;

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	void _onScrollV(int y);

	int _cxChar;
	int _cyChar;
	int _cxCaps;

	int _cyClient;
	int _iVscrollPos;
};

}

#endif // SGE_OS_WINDOWS