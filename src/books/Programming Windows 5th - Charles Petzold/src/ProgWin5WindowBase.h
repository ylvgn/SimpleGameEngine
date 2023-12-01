#pragma once

#include "MyCommon.h"
#include <sge_core/native_ui/base/NativeUIWindow_Base.h>

#if SGE_OS_WINDOWS

namespace sge {

class ProgWin5WindowBase : public NativeUIWindow_Base {
	using Base = NativeUIWindow_Base;
	using This = ProgWin5WindowBase;
protected:

	using WndProcHandler = LRESULT(CALLBACK*) (HWND, UINT, WPARAM, LPARAM);

	virtual void onCreate(CreateDesc& desc) override;
	virtual WndProcHandler onGetWndProc() { return nullptr; }

	HWND _hwnd;
};

#endif

}