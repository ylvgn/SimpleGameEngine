#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_Chap04_Common.h"

namespace sge {

class PW5_MySysMets2 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MySysMets2;
	using DefinationRemarks = PW5_MyDefinationRemarks;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
private:
	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);
	static WNDPROC s_defaultWndProc;

	void _onScrollV(int y);

	int  _viewportH;
	int  _scrollPosV;
	
	const DefinationRemarks* _dm = nullptr;

	MyTextMetrics _tm;
};

}

#endif // SGE_OS_WINDOWS