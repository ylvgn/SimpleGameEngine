#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

RenderContext::RenderContext(CreateDesc& desc) {
}

void RenderContext::onPostCreate() {
	//_imgui.create(); TODO TEMP just for opengl
}

void RenderContext::beginRender() {
	//_imgui.onBeginRender(this); TODO TEMP just for opengl
	onBeginRender();
}

void RenderContext::endRender() {
	onEndRender();
	//_imgui.onEndRender(this);  TODO TEMP just for opengl
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

} // namespace