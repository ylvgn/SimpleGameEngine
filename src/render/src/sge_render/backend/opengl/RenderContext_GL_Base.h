#pragma once

#include <sge_render/RenderContext.h>
#include "Render_GL_Common.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {
#if 0
#pragma mark ========= GLObject ============
#endif
class GLObject : public NonCopyable {
public:
	virtual ~GLObject() { SGE_ASSERT(_handle == 0); }

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
	using Util = GLUtil;

	RenderContext_GL_Base(Base::CreateDesc& desc);

protected:
	class VertexArray : public GLObject {
	public:
		~VertexArray();
		void bind();
		void unbind();
	};

	VertexArray	_vao;

}; // RenderContext_GL_Base

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL