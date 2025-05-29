#pragma once

#include "Render_GL_Common.h"
#include <sge_render/buffer/RenderGpuStorageBuffer.h>
#include "RenderGpuBuffer_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

class RenderGpuStorageBuffer_GL : public RenderGpuStorageBuffer {
	using This = RenderGpuStorageBuffer_GL;
	using Base = RenderGpuStorageBuffer;
	using Util = GLUtil;
public:
	using CreateDesc = RenderGpuStorageBuffer_CreateDesc;

	RenderGpuBuffer_GL* gpuBuffer() { return static_cast<RenderGpuBuffer_GL*>(Base::gpuBuffer()); }

	RenderGpuStorageBuffer_GL(CreateDesc& desc) : Base(desc) {}

}; // RenderGpuStorageBuffer_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL