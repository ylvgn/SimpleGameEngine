#include "PW5_NativeUIApp_Win32.h"

#if SGE_OS_WINDOWS

namespace sge {

void PW5_NativeUIApp_Win32::onRun() {
	Base::onRun();

	// message loop: when _msg.message == WM_QUIT, GetMessage will break the loop
	while (GetMessage(&_win32_msg, NULL, 0, 0)) // the 2,3,4 arguments is NULL means wants all messages for all windows created by this program
	{
		TranslateMessage(&_win32_msg); // Translates some keyboard messages
		DispatchMessage(&_win32_msg);  // Sends a message to a window procedure
	}

	willQuit();
}

void PW5_NativeUIApp_Win32::onQuit() {
	Base::onQuit();
	::PostQuitMessage(_exitCode);
}

}

#endif