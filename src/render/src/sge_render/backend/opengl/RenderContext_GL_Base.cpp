#pragma once

#include "RenderContext_GL_Base.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {
#if 0
#pragma mark ========= RenderContext_GL_Base::VertexArray ============
#endif
RenderContext_GL_Base::VertexArray::~VertexArray() {
	if (_handle) {
		glDeleteVertexArrays(1, &_handle);
		_handle = 0;
	}
}

void RenderContext_GL_Base::VertexArray::bind() {
	if (!_handle) {
		glGenVertexArrays(1, &_handle);
		Util::throwIfError();
	}
	glBindVertexArray(_handle);
}

#if 0
#pragma mark ========= RenderContext_GL_Base ============
#endif
RenderContext_GL_Base::RenderContext_GL_Base(CreateDesc& desc) : Base(desc) {
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL