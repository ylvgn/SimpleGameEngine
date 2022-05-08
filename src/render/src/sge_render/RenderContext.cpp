#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

	RenderContext* RenderContext::create(CreateDesc& desc) {
		return Renderer::current()->createRenderContext(desc);
	}

	RenderContext::RenderContext(CreateDesc& desc) {

	}

	// test
	void RenderContext::render()
	{
		beginRender();
		clearColorAndDepthBuffer();
		draw();
		swapBuffers();
		endRender();
	}
}