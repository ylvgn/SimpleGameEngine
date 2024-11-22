#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

RenderContext::RenderContext(CreateDesc& desc) {
}

void RenderContext::onPostCreate() {
	auto* p = Renderer::instance();
	{
		p->stockTextures.white		= p->createSolidColorTexture2D(Color4b(255, 255, 255, 255));
		p->stockTextures.black		= p->createSolidColorTexture2D(Color4b(0,   0,   0,   255));
		p->stockTextures.red		= p->createSolidColorTexture2D(Color4b(255, 0,   0,   255));
		p->stockTextures.green		= p->createSolidColorTexture2D(Color4b(0,   255, 0,   255));
		p->stockTextures.blue		= p->createSolidColorTexture2D(Color4b(0,   0,   255, 255));
		p->stockTextures.magenta	= p->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
		p->stockTextures.error		= p->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
	}

	ImGui_SGE::CreateDesc desc;
	_imgui.create(desc);
}

void RenderContext::beginRender() {
	_imgui.onBeginRender(this);
	onBeginRender();
}

void RenderContext::endRender() {
	onEndRender();
	_imgui.onEndRender(this);
}

void RenderContext::setFrameBufferSize(Vec2f newSize) {
	if (_frameBufferSize == newSize)
		return;

	_frameBufferSize = newSize;
	onSetFrameBufferSize(newSize);
}

void RenderContext::drawUI(RenderRequest& req) {
	_imgui.onDrawUI(req);
}

bool RenderContext::onUIMouseEvent(UIMouseEvent& ev) {
	return _imgui.onUIMouseEvent(ev);
}

void RenderContext::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	_imgui.onUIKeyboardEvent(ev);
}

} // namespace sge