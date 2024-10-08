#pragma once

#if SGE_RENDER_HAS_OPENGL

#include "RenderContext_GL_Base.h"

namespace sge {

#if 0
#pragma mark ========= GLVertexArray ============
#endif
void RenderContext_GL_Base::GLVertexArray::bind() {
	if (!_gl) {
		glGenVertexArrays(1, &_gl);
		Util::throwIfError();
	}
	glBindVertexArray(_gl);
	Util::throwIfError();
}

void RenderContext_GL_Base::GLVertexArray::_destroy() {
	if (_gl) {
		glDeleteVertexArrays(1, &_gl);
		_gl = 0;
	}
}

#if 0
#pragma mark ========= RenderContext_GL_Base ============
#endif
RenderContext_GL_Base::RenderContext_GL_Base(CreateDesc& desc)
	: Base(desc)
{
}

}
#endif // SGE_RENDER_HAS_OPENGL