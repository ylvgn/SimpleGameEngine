#pragma once
#include "Render_GL_Common.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderGpuBuffer_GL : public RenderGpuBuffer {
	using Base = RenderGpuBuffer;
	using Util = GLUtil;
public:
	void destroy();

	GLuint glBuf() const { return _p; }
	GLenum glBufTarget() const { return Util::getGlBufferBindingTarget(_desc.type); }

	void glBind();
	void glUnbind();

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onUploadToGpu(ByteSpan data, size_t offset) override;
private:
	GLuint _p = 0;

}; // RenderGpuBuffer_GL

} // namespace