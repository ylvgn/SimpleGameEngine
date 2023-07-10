#include "NativeUIApp_Win32.h"
#include "../NativeUI.h"
#include "../../string/UtfUtil.h"

#if SGE_OS_WINDOWS

namespace sge {

void NativeUIApp_Win32::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);
}

void NativeUIApp_Win32::onRun() {
	Base::onRun();
	_tickCount = GetTickCount64();

	// https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues
	while (_win32_msg.message != WM_QUIT) {
		if (PeekMessage(&_win32_msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&_win32_msg);
			DispatchMessage(&_win32_msg);
		}
		else {
			u64 thisTickCount = GetTickCount64();
			float deltaTime = static_cast<float>(thisTickCount - _tickCount) * 0.001f;
			_tickCount = thisTickCount;
			update(deltaTime);
		}
	}

	willQuit();
}

void NativeUIApp_Win32::onQuit() {
	Base::onQuit();

	::PostQuitMessage(_exitCode);
}

}

#endif
