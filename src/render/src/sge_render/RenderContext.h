#pragma once

#include <sge_core/base/Object.h>
#include "command/RenderCommand.h"
#include "ImGui_SGE.h"

namespace sge {

class RenderContext_CreateDesc {
public:
	NativeUIWindow* window = nullptr;
};

class RenderContext_Statistics { // TODO
	using This = RenderContext_Statistics;
public:
	int drawCall = 0;
	u64 verts = 0;
	u64 tris = 0;

	void clean() { memset(this, 0, sizeof(This)); }
};

#if 0
#pragma mark ========= RenderContext ============
#endif
class RenderContext : public Object {
public:
	using CreateDesc = RenderContext_CreateDesc;
	using Statistics = RenderContext_Statistics;

	virtual ~RenderContext() = default;

	void beginRender();
	void endRender();

	void setFrameBufferSize(Vec2f newSize);
	const Vec2f& frameBufferSize() const { return _frameBufferSize; }

	const Statistics& statistics() const { return _statistics; }

	void commit(RenderCommandBuffer& cmdBuf) { _statistics.clean(); onCommit(cmdBuf); }

	void onPostCreate();
	void drawUI(RenderRequest& req);
	bool onUIMouseEvent(UIMouseEvent& ev);
	void onUIKeyboardEvent(UIKeyboardEvent& ev);

protected:
	RenderContext(CreateDesc& desc); // please create from 'Renderer::createRenderContext'

	virtual void onBeginRender() {}
	virtual void onEndRender() {}

	virtual void onSetFrameBufferSize(const Vec2f& newSize) {};
	virtual void onCommit(RenderCommandBuffer& cmdBuf) {};

	template<class IMPL>
	void _dispatch(IMPL* impl, RenderCommandBuffer& cmdBuf) {
		using Cmd = RenderCommandType;

		#define CMD_CASE(E) \
			case Cmd::E: { \
				auto* c = static_cast<RenderCommand_##E*>(cmd); \
				impl->onCmd_##E(*c); \
			} break; \
		//----

		for (auto* cmd : cmdBuf.commands()) {
			switch (cmd->type()) {
				CMD_CASE(ClearFrameBuffers)
				CMD_CASE(SwapBuffers)
				CMD_CASE(DrawCall)
				CMD_CASE(SetScissorRect)
				default:
					throw SGE_ERROR("unhandled command");
			}
		}

		#undef CMD_CASE
	}

	ImGui_SGE	_imgui;
	Vec2f		_frameBufferSize {0,0};
	Statistics	_statistics;
}; // RenderContext_CreateDesc

} // namespace sge