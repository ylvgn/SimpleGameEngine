#if SGE_OS_WINDOWS
#include "PW5_ScrnSize.h"

namespace sge {

int CDECL PW5_ScrnSize::s_messageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...) {
	TCHAR   szBuffer[220];
	va_list pArgList;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	va_start(pArgList, szFormat); // pArgList = (char *) &szFormat + sizeof (szFormat) ;

#if 0
	// The last argument to wvsprintf points to the arguments
	// _vsntprintf (Warning C6053): https://learn.microsoft.com/en-us/cpp/code-quality/c6053?view=msvc-170
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
#else
	// corrects this warning using safe string manipulation: 
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
#endif

	// The va_end macro just zeroes out pArgList for no good reason
	va_end(pArgList);

	return MessageBox(NULL, szBuffer, szCaption, 0);
}

HMONITOR PW5_ScrnSize::s_getPrimaryMonitor() {
	POINT ptZero = { 0, 0 };
	return ::MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
}

float PW5_ScrnSize::s_getMonitorScalingRatio(HMONITOR monitor) {
	MONITORINFOEX info;
	g_bzero(info);
	info.cbSize = sizeof(MONITORINFOEX);
	::GetMonitorInfo(monitor, &info);

	DEVMODE devmode;
	g_bzero(devmode);
	devmode.dmSize = sizeof(DEVMODE);
	::EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
	return static_cast<float>(devmode.dmPelsWidth) / (info.rcMonitor.right - info.rcMonitor.left);
}

void PW5_ScrnSize::onCreate(CreateDesc& desc) {
	int cyCaption = ::GetSystemMetrics(SM_CYCAPTION); // The height of a caption area, in pixels.
	s_messageBoxPrintf(
		TEXT("SM_CYCAPTION"),
		TEXT("caption height: %d"),
		cyCaption
	);

	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN); // 'SM' -> System Metrics, `cx` -> x length, c stands for 'count'
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN); // The height of the screen primary display monitor in pixels (main display monitor)

	// 'Hxxx' -> Handle xxx
		// A handle is simply a number (usually 32 bits in size) that refers to an object
		// The actual value of the handle is unimportant. just kind of used in conventional C or MS−DOS programming.
	HMONITOR hMonitor = s_getPrimaryMonitor();
	float scale		  = s_getMonitorScalingRatio(hMonitor);

	SGE_LOG("Monitor Scaling Ratio={}", scale);

	s_messageBoxPrintf(
		TEXT("GetSystemMetrics With Scale"),
		TEXT("screen physics size: %.0f x %.0f"),
		cxScreen * scale, cyScreen * scale
	);

	s_messageBoxPrintf(
		TEXT("SM_CXSCREEN x SM_CYSCREEN"),
		TEXT("The screen is %i pixels wide by %i pixels high."),
		cxScreen, cyScreen
	);
}

}

#endif // SGE_OS_WINDOWS