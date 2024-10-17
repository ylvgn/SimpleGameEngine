#pragma once

#if SGE_RENDER_HAS_OPENGL

#include <sge_render/RenderContext.h>
#include "Render_GL_Common.h"

namespace sge {
#if 0
#pragma mark ========= GLObject ============
#endif
class GLObject : public NonCopyable {
public:
	virtual ~GLObject() noexcept { SGE_ASSERT(_handle == 0); }
	bool isValid() const { return _handle != 0; }
protected:
	GLuint _handle = 0;
}; // GLObject

#if 0
#pragma mark ========= RenderContext_GL_Base ============
#endif
class RenderContext_GL_Base : public RenderContext {
	using Base = RenderContext;
public:
	RenderContext_GL_Base(CreateDesc& desc);

protected:
	using Util = GLUtil;

	class VertexArray : public GLObject {
	public:
		~VertexArray();
		void bind();
		void unbind() { glBindVertexArray(0); }
	};

	VertexArray	_vao;
}; // RenderContext_GL_Base

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL