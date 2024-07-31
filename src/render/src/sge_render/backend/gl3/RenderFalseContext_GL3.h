#pragma once

#if SGE_OS_WINDOWS
	#include "OpenGLFalseContext_Win32.h"
#endif

namespace sge {

#if SGE_OS_WINDOWS
	using OpenGLFalseContext_Impl = OpenGLFalseContext_Win32;
#else
	#error "unsupported platform"
#endif

class RenderFalseContext_GL3 : public OpenGLFalseContext_Impl {
	
};

}