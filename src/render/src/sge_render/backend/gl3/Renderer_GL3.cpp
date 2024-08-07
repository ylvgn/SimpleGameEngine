#include "Renderer_GL3.h"
#include "RenderContext_GL3.h"
#include "RenderGpuBuffer_GL3.h"

namespace sge {

Renderer_GL3::Renderer_GL3(CreateDesc& desc) {

}

SPtr<RenderContext> Renderer_GL3::onCreateContext(RenderContext_CreateDesc& desc) {
	return new RenderContext_GL3(desc);
}

SPtr<RenderGpuBuffer> Renderer_GL3::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) {
	SPtr<RenderGpuBuffer> p = new RenderGpuBuffer_GL3();
	p->create(desc);
	return p;
}

}
