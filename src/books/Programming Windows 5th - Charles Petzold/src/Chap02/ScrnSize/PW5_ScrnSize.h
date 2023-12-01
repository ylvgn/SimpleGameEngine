#pragma once

#include "ProgWin5WindowBase.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_ScrnSize : public ProgWin5WindowBase {
	using This = PW5_ScrnSize;
	using Base = ProgWin5WindowBase;
protected:
	virtual void onCreate(CreateDesc& desc) override;

private:
	static int s_messageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...);
	static float s_getMonitorScalingRatio(HMONITOR monitor);
	static HMONITOR s_getPrimaryMonitor();
};

#endif

}