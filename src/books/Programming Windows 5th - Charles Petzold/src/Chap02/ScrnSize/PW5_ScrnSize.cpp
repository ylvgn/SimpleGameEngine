#include "PW5_ScrnSize.h"

namespace sge {

int CDECL PW5_ScrnSize::s_messageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...) {
	TCHAR   szBuffer[1024];
	va_list pArgList;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	// pArgList = (char *) &szFormat + sizeof (szFormat) ;
	va_start(pArgList, szFormat);

	// The last argument to wvsprintf points to the arguments

	//_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	// _vsntprintf is deprecated
	// corrects this warning using safe string manipulation: https://learn.microsoft.com/en-us/cpp/code-quality/c6053?view=msvc-170
	_vsnwprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);

	// The va_end macro just zeroes out pArgList for no good reason
	va_end(pArgList);

	return MessageBox(NULL, szBuffer, szCaption, 0); //  prefix "MB" -> message box
}

HMONITOR PW5_ScrnSize::s_getPrimaryMonitor() {
	POINT ptZero = { 0, 0 };
	return MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
}

float PW5_ScrnSize::s_getMonitorScalingRatio(HMONITOR monitor)
{
	MONITORINFOEX info;
	info.cbSize = sizeof(info);
	GetMonitorInfo(monitor, &info);

	DEVMODE devmode;
	devmode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
	return (float)(devmode.dmPelsWidth) / (info.rcMonitor.right - info.rcMonitor.left);
}

void PW5_ScrnSize::onCreate(CreateDesc& desc) {
	int cyCaption = GetSystemMetrics(SM_CYCAPTION); // The height of a caption area, in pixels.
	s_messageBoxPrintf(TEXT("PW5_ScrnSize"),
		TEXT("caption height = %d"),
		cyCaption
	);

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	auto hMonitor	= s_getPrimaryMonitor();
	float scale		= s_getMonitorScalingRatio(hMonitor);

	s_messageBoxPrintf(TEXT("PW5_ScrnSize"),
		TEXT("screen physics size: %.0f x %.0f"),
		cxScreen * scale, cyScreen * scale
	);

	s_messageBoxPrintf(TEXT("PW5_ScrnSize"),
		TEXT("The screen is %i pixels wide by %i pixels high."),
		cxScreen, cyScreen
	);
}

}