#pragma once

#include "Render_GL_Common.h"

#if SGE_RENDER_HAS_OPENGL

#include <sge_render/Renderer.h>

namespace sge {

class Renderer_GL : public Renderer {
	using Base = Renderer;
	using Util = GLUtil;
public:
	Renderer_GL(CreateDesc& desc);

	static Renderer_GL* instance() { return static_cast<Renderer_GL*>(s_instance); }

protected:
	sgeRenderer_InterfaceFunctions(GL);
}; // Renderer_GL

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL