#pragma once

#if SGE_RENDER_HAS_OPENGL

#include <sge_render/Renderer.h>
#include "Render_GL_Common.h"

namespace sge {

class Renderer_GL : public Renderer {
	using Base = Renderer;
	using Util = GLUtil;
public:
	Renderer_GL(CreateDesc& desc);
	static Renderer_GL* current() { return static_cast<Renderer_GL*>(s_instance); }

protected:
	virtual SPtr<RenderContext>		onCreateContext(RenderContext_CreateDesc& desc) override;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) override;
	virtual SPtr<Shader>			onCreateShader(StrView filename) override;
	virtual SPtr<Material>			onCreateMaterial() override;

	virtual SPtr<Texture2D>			onCreateTexture2D(Texture2D_CreateDesc& desc) { return nullptr; } // TODO
};

}

#endif // SGE_RENDER_HAS_OPENGL