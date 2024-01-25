#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {
/*
	-sprintf
	-GetSystemMetrics
*/

class PW5_ScrnSize : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_ScrnSize;
protected:
	virtual void onCreate(CreateDesc& desc) override;
private:
	static CDECL int s_messageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...);
	static HMONITOR  s_getPrimaryMonitor();
	static float	 s_getMonitorScalingRatio(HMONITOR monitor);
};

} // namespace

#endif // SGE_OS_WINDOWS