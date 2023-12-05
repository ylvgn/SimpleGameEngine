#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {
/*
	-sprintf
	-GetSystemMetrics
*/

class PW5_ScrnSize : public ProgWin5WindowBase {
	using This = PW5_ScrnSize;
	using Base = ProgWin5WindowBase;
protected:
	virtual void onCreate(CreateDesc& desc) override;
private:
	static CDECL int s_messageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...);
	static HMONITOR  s_getPrimaryMonitor();
	static float	 s_getMonitorScalingRatio(HMONITOR monitor);
};

} // namespace

#endif // SGE_OS_WINDOWS