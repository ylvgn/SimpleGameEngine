#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_NativeUI.h"

namespace sge {

struct MySysMetrics {
     int		iIndex;
     wchar_t*	szLabel;
	 wchar_t*	szDesc;
};

extern MySysMetrics g_sysmetrics[];
extern size_t		g_sysmetricsCount;

}

#endif