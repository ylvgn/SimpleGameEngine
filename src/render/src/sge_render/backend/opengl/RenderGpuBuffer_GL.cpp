#include "RenderGpuBuffer_GL.h"
#include "Renderer_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

void RenderGpuBuffer_GL::onCreate(CreateDesc& desc)  {
	_destroy();

	if (desc.bufferSize <= 0)	throw SGE_ERROR("buffer size <= 0");
	if (desc.stride <= 0)		throw SGE_ERROR("stride <= 0");

	_target = Util::getGlBufferBindingTarget(_desc.type);

	glGenBuffers(1, &_handle);
	bind();
	GLenum usage = GL_DYNAMIC_DRAW;
	glBufferData(_target, static_cast<GLsizeiptr>(desc.bufferSize), nullptr, usage);
	unbind();

	Util::throwIfError();
}

void RenderGpuBuffer_GL::onUploadToGpu(ByteSpan data, size_t offset) {
	bind();

#if false // use which one is ok
	u8* dst = reinterpret_cast<u8*>(glMapBuffer(_target, GL_WRITE_ONLY));
	memcpy(dst + offset, data.data(), data.size());
	glUnmapBuffer(_target);
#elif 1 // GPU side move offset
	auto* p = glMapBufferRange(_target, offset, data.size_bytes(), GL_MAP_WRITE_BIT);
	if (!p) throw SGE_ERROR("glMapBufferRange");
	u8* dst = reinterpret_cast<u8*>(p);
	memcpy(dst, data.data(), data.size_bytes());
	glUnmapBuffer(_target);
#else
	glBufferSubData(_target, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
#endif

	unbind();
	Util::throwIfError();
}

void RenderGpuBuffer_GL::_destroy() {
	if (_handle) {
		glDeleteBuffers(1, &_handle);
		_handle = 0;
	}
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL