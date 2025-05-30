#include "RenderCommand.h"
#include "../RenderContext.h"

namespace sge {

void RenderCommandBuffer::reset(RenderContext* ctx) {
	for (auto* cmd : _commands) {
		cmd->~RenderCommand();
	}
	_allocator.clear();
	_commands.clear();

	_scissorRect = Rect2f(Vec2f::s_zero(), ctx->frameBufferSize());
}

} // namespace sge