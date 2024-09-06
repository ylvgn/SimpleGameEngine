#include "Renderer_GL3.h"
#include "RenderContext_GL3.h"
#include "RenderGpuBuffer_GL3.h"
#include "Material_GL3.h"

namespace sge {

Renderer_GL3::Renderer_GL3(CreateDesc& desc) {

}

SPtr<RenderContext> Renderer_GL3::onCreateContext(RenderContext_CreateDesc& desc) {
#if 0 // TODO not supported ImGui impl in OpenGL yet
	SPtr<RenderContext> p = new RenderContext_GL3(desc);
	p->onPostCreate();
	return p;
#else
	return new RenderContext_GL3(desc);
#endif
}

SPtr<RenderGpuBuffer> Renderer_GL3::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) {
	SPtr<RenderGpuBuffer> p = new RenderGpuBuffer_GL3();
	p->create(desc);
	return p;
}

SPtr<Shader> Renderer_GL3::onCreateShader(StrView filename) {
	return new Shader_GL3(filename);
}

SPtr<Material> Renderer_GL3::onCreateMaterial() {
	return new Material_GL3();
}

}
