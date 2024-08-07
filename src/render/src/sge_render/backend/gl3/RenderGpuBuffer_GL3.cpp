#include "RenderGpuBuffer_GL3.h"
#include "Renderer_GL3.h"

namespace sge {

void RenderGpuBuffer_GL3::destroy() {
	if (_p) {
		glDeleteBuffers(1, &_p);
		_p = 0;
	}
}

void RenderGpuBuffer_GL3::onCreate(CreateDesc& desc)  {
	destroy();

	if (desc.bufferSize <= 0) throw SGE_ERROR("buffer size = 0");
	if (desc.stride <= 0) throw SGE_ERROR("stride == 0");

	auto target = Util::getGlBufferBindingTarget(_desc.type);
	glGenBuffers(1, &_p);
	glBindBuffer(target, _p);
	glBufferData(target, static_cast<GLsizeiptr>(desc.bufferSize), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(target, 0);

	Util::throwIfError();
}

void RenderGpuBuffer_GL3::onUploadToGpu(ByteSpan data, size_t offset) {
	auto target = Util::getGlBufferBindingTarget(_desc.type);
	glBindBuffer(target, _p);

#if 1 // which is better ???
	u8* dst = reinterpret_cast<u8*>(glMapBuffer(target, GL_WRITE_ONLY));
	memcpy(dst + offset, data.data(), data.size());
	glUnmapBuffer(target);
#else
	glBufferSubData(target, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
#endif

	Util::throwIfError();
}

}