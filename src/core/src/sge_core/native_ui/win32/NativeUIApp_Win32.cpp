#include "NativeUIApp_Win32.h"
#include "../NativeUI.h"
#include "../../string/UtfUtil.h"

#if SGE_OS_WINDOWS

namespace sge {

void NativeUIApp_Win32::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);
	_lastTick = 0;
}

void NativeUIApp_Win32::onRun() {
	Base::onRun();

	// https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues
	while (_win32_msg.message != WM_QUIT) {
		if (PeekMessage(&_win32_msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&_win32_msg);
			DispatchMessage(&_win32_msg);
		}
		else {
			ULONGLONG thisTick = GetTickCount64();
			float deltaTime = static_cast<float>(thisTick - _lastTick) * 0.001f;
			_lastTick = thisTick;
			onUpdate(deltaTime);
		}
	}

	willQuit();
}

void NativeUIApp_Win32::onUpdate(float dt) {
	Base::onUpdate(dt);
}

void NativeUIApp_Win32::onQuit() {
	Base::onQuit();

	::PostQuitMessage(_exitCode);
}

}

#endif
