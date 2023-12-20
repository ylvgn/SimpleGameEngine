#pragma once

#include "MyCommon.h"

namespace sge {

#if SGE_OS_WINDOWS

struct MySysMetrics {
     int     iIndex;
     TCHAR*  szLabel;
     TCHAR*  szDesc;
};

extern MySysMetrics g_sysmetrics[];
extern size_t		g_sysmetricsCount;

#endif

}