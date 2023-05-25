#pragma once

#include "../base/NativeUIWindow_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

class NativeUIWindow_Win32 : public NativeUIWindow_Base {
	using This = NativeUIWindow_Win32;
	using Base = NativeUIWindow_Base;
public:

	virtual void onCreate(CreateDesc& desc) override;
	virtual void onSetWindowTitle(StrView title) override;
	virtual void onSetCursor(UIMouseCursorType type) override;
	virtual void onDrawNeeded() override;

	const HWND hwnd() const { return _hwnd; }
	const HDC hdc() const { return GetDC(_hwnd); }

protected:
	HWND _hwnd = nullptr;

private:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	LRESULT _handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIKeyboardEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	UIEventModifier _getWin32Modifier();
};

} // namespace

#endif // SGE_OS_WINDOWS