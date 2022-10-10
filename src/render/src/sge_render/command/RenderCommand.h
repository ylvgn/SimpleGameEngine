#pragma once

#include <sge_render/vertex/Vertex.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/shader/Material.h>

namespace sge {

class RenderMesh;
class RenderSubMesh;

enum class RenderCommandType {
	None,
	ClearFrameBuffers,
	SwapBuffers,
	DrawCall,
	SetScissorRect,
};

class RenderCommand : NonCopyable {
	using Type = RenderCommandType;
public:

	RenderCommand(Type type) : _type(type) {}
	virtual ~RenderCommand() {}

	Type type() const { return _type; }

#if _DEBUG
	SrcLoc	debugLoc;
#endif

private:
	Type _type = Type::None;
}; // RenderCommand

class RenderCommand_ClearFrameBuffers : public RenderCommand {
	using Base = RenderCommand;
	using This = RenderCommand_ClearFrameBuffers;
public:
	RenderCommand_ClearFrameBuffers() : Base(Type::ClearFrameBuffers) {}

	This& setColor(const Color4f& color_)	{ color = color_; return *this; }
	This& setDepth(float depth_)			{ depth = depth_; return *this; }

	This& dontClearColor() { color.reset(); return *this; }
	This& dontClearDepth() { depth.reset(); return *this; }

	Opt<Color4f> color = Color4f(1, 1, 1, 1);
	Opt<float>   depth = 1;

}; // RenderCommand_ClearFrameBuffers

class RenderCommand_SwapBuffers : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_SwapBuffers() : Base(Type::SwapBuffers) {}
}; // RenderCommand_SwapBuffers

class RenderCommand_DrawCall : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_DrawCall() : Base(Type::DrawCall) {}

	RenderPrimitiveType	 primitive = RenderPrimitiveType::None;
	const VertexLayout* vertexLayout = nullptr;

	SPtr<RenderGpuBuffer> vertexBuffer;
	RenderDataType indexType = RenderDataType::UInt16;
	SPtr<RenderGpuBuffer> indexBuffer;

	SPtr<Material>			material;
	size_t					materialPassIndex = 0;

	MaterialPass* getMaterialPass() {
		return material ? material->getPass(materialPassIndex) : nullptr;
	}

	size_t indexOffset	= 0;
	size_t vertexOffset = 0;
	size_t vertexCount	= 0;
	size_t indexCount	= 0;
}; // RenderCommand_DrawCall

class RenderCommand_SetScissorRect : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_SetScissorRect() : Base(Type::SetScissorRect) {}
	Rect2f rect;
}; // RenderCommand_SetScissorRect

class RenderCommandBuffer : public NonCopyable {
public:

	void reset(RenderContext* ctx);

	Span<RenderCommand*>	commands() { return _commands; }

	const Rect2f& scissorRect() const { return _scissorRect; }

	void setScissorRect(const Rect2f& rect) {
		_scissorRect = rect;
		auto* cmd = newCommand<RenderCommand_SetScissorRect>();
		cmd->rect = rect;
	}

	RenderCommand_ClearFrameBuffers*	clearFrameBuffers()	{ return newCommand<RenderCommand_ClearFrameBuffers>();	}
	RenderCommand_SwapBuffers*			swapBuffers()		{ return newCommand<RenderCommand_SwapBuffers>();		}
	RenderCommand_DrawCall*				addDrawCall()		{ return newCommand<RenderCommand_DrawCall>();			}

	template<class CMD>
	CMD* newCommand() {
		auto* buf = _allocator.allocate(sizeof(CMD));
		auto* cmd = new(buf) CMD();
		_commands.emplace_back(cmd);
		return cmd;
	}

private:
	Vector<RenderCommand*, 64>	_commands;
	LinearAllocator _allocator;
	Rect2f _scissorRect;
}; // RenderCommandBuffer

} // namespace

