#pragma once

#include <sge_core.h>

#if SGE_OS_WINDOWS

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdint.h>

namespace sge {

template<class T> inline constexpr
void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

#endif // SGE_OS_WINDOWS
}