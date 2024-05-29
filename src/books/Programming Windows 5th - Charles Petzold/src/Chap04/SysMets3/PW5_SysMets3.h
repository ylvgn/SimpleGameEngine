#pragma once

#include "../PW5_Chap04_Common.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_SysMets3 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_SysMets3;
	using ScrollInfo = decltype(Base::_vScrollInfo)::element_type;
protected:
	virtual void onCreate(CreateDesc& desc) override;
private:
	static LRESULT CALLBACK s_wndProc(HWND, UINT, WPARAM, LPARAM);

	SGE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	UPtr<ScrollInfo> _siV;
	UPtr<ScrollInfo> _siH;
};

}

#endif // SGE_OS_WINDOWS