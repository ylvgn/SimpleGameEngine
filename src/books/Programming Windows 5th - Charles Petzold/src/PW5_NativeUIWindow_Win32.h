#pragma once

#if SGE_OS_WINDOWS

#include "PW5_Common_Win32.h"
#include <sge_core/native_ui/base/NativeUIWindow_Base.h>

namespace sge {

class PW5_NativeUIWindow_Win32 : public NativeUIWindow_Base {
	using Base = NativeUIWindow_Base;
	using This = PW5_NativeUIWindow_Win32;
	using Util = Win32Util;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onOpen() {}
	virtual void onDraw() final;
	virtual void onPaint(ScopedPaintStruct& ps) {}

	virtual void onSetWindowTitle(StrView title) override;
	virtual void onSetWindowSize(const Rect2i& xywh) override;

private:
	static SGE_INLINE This*	s_getThis(HWND hwnd) {
		return static_cast<This*>(reinterpret_cast<LPVOID>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));
	}

	static LRESULT CALLBACK	s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT _handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
protected:
	HWND _hwnd = nullptr;
};

}

#endif // SGE_OS_WINDOWS