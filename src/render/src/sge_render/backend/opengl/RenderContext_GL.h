#pragma once

#if SGE_RENDER_HAS_OPENGL

#if SGE_OS_WINDOWS
	#include "RenderContext_GL_Win32.h"

	namespace sge {
		using RenderContext_GL_Impl = RenderContext_GL_Win32;
	}

#else
	#error "unsupported platform"
#endif // SGE_OS_WINDOWS


namespace sge {

class RenderContext_GL : public RenderContext_GL_Impl {
	using Base = RenderContext_GL_Impl;
public:
	RenderContext_GL(CreateDesc& desc) : Base(desc) {}
}; // RenderContext_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL