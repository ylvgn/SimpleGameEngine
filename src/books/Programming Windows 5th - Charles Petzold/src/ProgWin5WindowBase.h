#pragma once

#include <sge_core/native_ui/base/NativeUIWindow_Base.h>

#include "MyCommon.h"

#if SGE_OS_WINDOWS

#include <sge_core/native_ui/win32/Win32Util.h>

namespace sge {

class ProgWin5WindowBase : public NativeUIWindow_Base {
	using Base = NativeUIWindow_Base;
	using This = ProgWin5WindowBase;
protected:
	using WndProcHandler = LRESULT(CALLBACK*) (HWND, UINT, WPARAM, LPARAM);

	virtual void onCreate(CreateDesc& desc) override;
	virtual void onSetWindowTitle(StrView title) override;

	virtual WndProcHandler onGetWndProc() { return nullptr; }

	static LRESULT CALLBACK s_WndProcDefault(HWND, UINT, WPARAM, LPARAM);

	static SGE_INLINE LONG_PTR	s_getWindowUserData(HWND hwnd)	{ return GetWindowLongPtr(hwnd, GWLP_USERDATA); }
	static SGE_INLINE This*		s_getThis(HWND hwnd)			{ return reinterpret_cast<This*>(s_getWindowUserData(hwnd)); }

	HWND _hwnd = nullptr;
};

}

#endif // SGE_OS_WINDOWS