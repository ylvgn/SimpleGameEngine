#pragma once

#include "PW5_Common.h"

#if SGE_OS_WINDOWS

#include "PW5_Common_Win32.h"
#include "PW5_NativeUIApp_Win32.h"
#include "PW5_NativeUIWindow_Win32.h"

namespace sge {
	using PW5_NativeUIApp_Impl = PW5_NativeUIApp_Win32;
	using PW5_NativeUIWindow_Impl = PW5_NativeUIWindow_Win32;
}

#endif // SGE_OS_WINDOWS


namespace sge {

class PW5_NativeUIApp : public PW5_NativeUIApp_Impl {
	using Base = PW5_NativeUIApp_Impl;
public:
	PW5_NativeUIApp();
	~PW5_NativeUIApp();

	static PW5_NativeUIApp* instance() { return s_instance; }

private:
	static PW5_NativeUIApp* s_instance;
};

class PW5_NativeUIWindow : public PW5_NativeUIWindow_Impl {
	using Base = PW5_NativeUIWindow_Impl;
	using This = PW5_NativeUIWindow;
protected:
	virtual void onCloseButton() override;
	virtual void onCreate(CreateDesc& desc) {
		Base::onCreate(desc);
		onPostCreate();
	}

	virtual void onPostCreate() {}
};

}