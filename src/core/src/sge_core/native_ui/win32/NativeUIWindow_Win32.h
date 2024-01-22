#pragma once

#include "../base/NativeUIWindow_Base.h"
#include "NativeUIScrollInfo_Win32.h"

#if SGE_OS_WINDOWS

namespace sge {

class NativeUIWindow_Win32 : public NativeUIWindow_Base {
	using This = NativeUIWindow_Win32;
	using Base = NativeUIWindow_Base;
public:
	using ScrollInfo = NativeUIScrollInfo_Win32;

	virtual void onCreate(CreateDesc& desc) override;
	virtual void onSetWindowTitle(StrView title) override;
	virtual void onSetCursor(UIMouseCursor type) override;
	virtual void onDrawNeeded() override;
	virtual void onScrollWindow(const Vec2i& delta) override;

	const HWND	hwnd()	const	{ return _hwnd; }

protected:
	virtual Base::ScrollInfo* onCreateScrollBar() override;

	HWND _hwnd = nullptr;

	UPtr<ScrollInfo> _hScrollInfo = nullptr;
	UPtr<ScrollInfo> _vScrollInfo = nullptr;

private:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	LRESULT _handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIKeyboardEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIScrollBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	UIEventModifier _getWin32Modifier();
};

} // namespace

#endif // SGE_OS_WINDOWS