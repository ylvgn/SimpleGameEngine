#include "RenderGpuBuffer_GL.h"
#include "Renderer_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

void RenderGpuBuffer_GL::destroy() {
	if (_p) {
		glDeleteBuffers(1, &_p);
		_p = 0;
	}
}

void RenderGpuBuffer_GL::glBind() {
	SGE_ASSERT(_p);
	glBindBuffer(glBufTarget(), _p);
}

void RenderGpuBuffer_GL::glUnbind() {
	glBindBuffer(glBufTarget(), 0);
}

void RenderGpuBuffer_GL::onCreate(CreateDesc& desc)  {
	destroy();

	if (desc.bufferSize <= 0)	throw SGE_ERROR("buffer size <= 0");
	if (desc.stride <= 0)		throw SGE_ERROR("stride <= 0");

	auto target = glBufTarget();
	glGenBuffers(1, &_p);
	glBindBuffer(target, _p);
	glBufferData(target, static_cast<GLsizeiptr>(desc.bufferSize), nullptr, GL_DYNAMIC_DRAW);
//	glBindBuffer(target, 0);

	Util::throwIfError();
}

void RenderGpuBuffer_GL::onUploadToGpu(ByteSpan data, size_t offset) {
	auto target = glBufTarget();
	glBindBuffer(target, _p);

#if 1 // use which one is ok
	u8* dst = reinterpret_cast<u8*>(glMapBuffer(target, GL_WRITE_ONLY));
	memcpy(dst + offset, data.data(), data.size());
	glUnmapBuffer(target);
#else
	glBufferSubData(target, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
#endif
//	glBindBuffer(target, 0);

	Util::throwIfError();
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL