#pragma once

#include <sge_render/buffer/RenderGpuBuffer.h>

#if SGE_RENDER_HAS_OPENGL

#include "Render_GL_Common.h"

namespace sge {

class RenderGpuBuffer_GL : public RenderGpuBuffer {
	using Base = RenderGpuBuffer;
	using Util = GLUtil;
public:
	~RenderGpuBuffer_GL() { _destroy(); }

	void bind()		{ glBindBuffer(_target, _handle); }
	void unbind()	{ glBindBuffer(_target, 0); }

	bool	isValid()	const { return _handle != 0; }
	GLuint	handle()	const { return _handle; }

protected:
	virtual void onCreate(CreateDesc& desc) final;
	virtual void onUploadToGpu(ByteSpan data, size_t offset) final;

private:
	void _destroy();

	GLenum _target = 0;
	GLuint _handle = 0;

}; // RenderGpuBuffer_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL