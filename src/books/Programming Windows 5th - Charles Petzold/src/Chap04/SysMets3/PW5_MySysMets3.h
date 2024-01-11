#pragma once

#include "../PW5_Chap04_Common.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_MySysMets3 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MySysMets3;
protected:
	virtual void onPostCreate() override;
	virtual void onOpen() override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onPaint(ScopedPaintStruct& ps) override;
private:
	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);
	static WNDPROC s_defaultWndProc;

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	void _onScrollHLine(bool isLeft);
	void _onScrollHPage(bool isLeft);
	void _onScrollH(bool isLeft, bool isPage);
	void _onScrollH(int newX);

	void _onScrollVLine(bool isUp);
	void _onScrollVPage(bool isUp);
	void _onScrollV(bool isUp, bool isPage);
	void _onScrollV(int newY);

	int _cxChar, _cyChar, _cxCaps;
	ScrollInfo _siV, _siH;
};

}

#endif // SGE_OS_WINDOWS