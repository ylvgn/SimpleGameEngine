#pragma once

#include "NeHeOGL_GL_Common.h"

#if SGE_OS_WINDOWS
	#include "NeHeOGL_NativeUIWindow_Win32.h"

	namespace sge {
		using NeHeOGL_NativeUIWindow = NeHeOGL_NativeUIWindow_Win32;
	}
#else
	#error "unsupported platform"

#endif