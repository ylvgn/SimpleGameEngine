#include "Renderer_GL.h"
#include "RenderContext_GL.h"
#include "RenderGpuBuffer_GL.h"
#include "Material_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

Renderer_GL::Renderer_GL(CreateDesc& desc) {
}

SPtr<RenderContext> Renderer_GL::onCreateContext(RenderContext_CreateDesc& desc) {
#if 0 // TODO not supported ImGui impl in OpenGL yet
	SPtr<RenderContext> p = new RenderContext_GL(desc);
	p->onPostCreate();
	return p;
#else
	return new RenderContext_GL(desc);
#endif
}

SPtr<RenderGpuBuffer> Renderer_GL::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) {
	SPtr<RenderGpuBuffer> p = new RenderGpuBuffer_GL();
	p->create(desc);
	return p;
}

SPtr<Shader> Renderer_GL::onCreateShader(StrView filename) {
	SPtr<Shader> p = new Shader_GL(filename);
	p->_internal_init();
	return p;
}

SPtr<Material> Renderer_GL::onCreateMaterial() {
	return new Material_GL();
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL