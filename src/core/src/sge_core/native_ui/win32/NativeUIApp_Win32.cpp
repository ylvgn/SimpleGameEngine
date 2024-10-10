#include "NativeUIApp_Win32.h"
#include "../NativeUI.h"
#include "../../string/UtfUtil.h"

#if SGE_OS_WINDOWS

namespace sge {

void NativeUIApp_Win32::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	setFps(_getMonitorDisplayFrequency());
}

void NativeUIApp_Win32::onRun() {
	Base::onRun();

	_tickCount			= GetTickCount64();
	_win32_msg.message	= static_cast<UINT>(~WM_QUIT);

	while (_win32_msg.message != WM_QUIT) {
		if (PeekMessage(&_win32_msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&_win32_msg);
			DispatchMessage(&_win32_msg);
		} else {
			auto thisTickCount = GetTickCount64();
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

DWORD NativeUIApp_Win32::_getMonitorDisplayFrequency() {
	POINT ptZero = { 0, 0 };
	HMONITOR hMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(hMonitor, &monitorInfo);

	DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);
	devMode.dmDriverExtra = 0;
	EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
	return devMode.dmDisplayFrequency;
}

}

#endif // SGE_OS_WINDOWS
