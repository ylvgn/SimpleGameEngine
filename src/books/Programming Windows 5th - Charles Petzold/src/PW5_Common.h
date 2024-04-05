#pragma once

#include <sge_core.h>
#include <sge_core/native_ui/NativeUI.h>

#if SGE_OS_WINDOWS
	#include <sge_core/native_ui/win32/Win32Util.h>
	#include <windowsx.h>
	#include <tchar.h>
#endif

namespace sge {

template<class T> inline void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

}