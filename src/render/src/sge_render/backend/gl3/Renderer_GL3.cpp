#include "Renderer_GL3.h"
#include "RenderContext_GL3.h"

namespace sge {

Renderer_GL3::Renderer_GL3(CreateDesc& desc) {

}

SPtr<RenderContext> Renderer_GL3::onCreateContext(RenderContext_CreateDesc& desc) {
	return new RenderContext_GL3(desc);
}

}
