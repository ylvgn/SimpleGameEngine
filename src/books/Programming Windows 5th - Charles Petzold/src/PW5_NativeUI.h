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
public:
	struct CreateDesc_Win32 {
		CreateDesc_Win32(Base::CreateDesc& desc, WNDCLASSEX& wc_)
			: baseDesc(desc)
			, wc(wc_)
			, cwlpParam(nullptr) {}

		Base::CreateDesc& baseDesc;
		WNDCLASSEX& wc;
		LPVOID cwlpParam = nullptr;
	};

	static void s_registerClass(HMODULE hInstance, const wchar_t* clsName, WNDPROC wndProc = nullptr);
	static void s_createWindow(CreateDesc_Win32& desc);
protected:
	virtual void onCloseButton() override {
		PW5_NativeUIApp::instance()->quit(0);
	}
};

}