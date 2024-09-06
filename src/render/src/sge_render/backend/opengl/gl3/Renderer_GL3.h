#pragma once

#if SGE_RENDER_HAS_GL3

#include <sge_render/Renderer.h>
#include "Render_GL3_Common.h"

namespace sge {

class Renderer_GL3 : public Renderer {
	using Base = Renderer;
	using Util = GL3Util;
public:
	Renderer_GL3(CreateDesc& desc);
	static Renderer_GL3* current() { return static_cast<Renderer_GL3*>(s_instance); }

protected:
	virtual SPtr<RenderContext>		onCreateContext(RenderContext_CreateDesc& desc) override;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) override;
	virtual SPtr<Shader>			onCreateShader(StrView filename) override;
	virtual SPtr<Material>			onCreateMaterial() override;

	virtual SPtr<Texture2D>			onCreateTexture2D(Texture2D_CreateDesc& desc) { return nullptr; } // TODO
};

}

#endif