#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

RenderContext::RenderContext(CreateDesc& desc) {

}

void RenderContext::setFrameBufferSize(Vec2f newSize) {
	if (_frameBufferSize == newSize)
		return;

	_frameBufferSize = newSize;
	onSetFrameBufferSize(newSize);
}

} // namespace