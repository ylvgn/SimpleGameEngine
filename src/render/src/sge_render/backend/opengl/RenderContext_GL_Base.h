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

protected:
	class VertexArray : public GLObject {
	public:
		~VertexArray();
		void bind();
	};

	VertexArray	_vao;
};

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL