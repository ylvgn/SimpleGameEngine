#pragma once

#if SGE_RENDER_HAS_OPENGL

#include <sge_render/RenderContext.h>
#include "Render_GL_Common.h"

namespace sge {

class RenderContext_GL_Base : public RenderContext {
	using Base = RenderContext;
public:
	using Util = GLUtil;

	RenderContext_GL_Base(CreateDesc& desc);

	class GLVertexArray : public GLObject {
	public:
		~GLVertexArray() { _destroy(); }
		void bind();
	private:
		void _destroy();
	};

protected:
	GLVertexArray	_vao;
};

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL