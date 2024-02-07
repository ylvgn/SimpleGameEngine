#pragma once

#include "NeheOGL_Common.h"

#if SGE_OS_WINDOWS
	#include "NeheOGL_NativeUIWindow_Win32.h"

	namespace sge {
		using NeheOGL_NativeUIWindow = NeheOGL_NativeUIWindow_Win32;
	}
#else
	#error "unsupported platform"

#endif