#pragma once
#include <sge_core.h>
#include "../vertex/Vertex.h"

namespace sge {

class RenderMesh;

enum class RenderCommandType {
	None,
	ClearFrameBuffers,
	SwapBuffers,
	DrawCall,
};

class RenderCommand : NonCopyable {
	using Type = RenderCommandType;
public:
	RenderCommand(Type type) : _type(type) {}
	virtual ~RenderCommand() {}

private:
	Type _type = Type::None;
}; // RenderCommand

class RenderCommand_ClearFrameBuffers : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_ClearFrameBuffers() : Base(Type::ClearFrameBuffers) {}
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
	//RenderDataType indexType = RenderDataType::UInt16;

	//SPtr<RenderGpuBuffer> vertexBuffer;

	//SPtr<RenderGpuBuffer>	indexBuffer;
	//SPtr<MaterialPass> materialPass;

	size_t vertexCount = 0;
	size_t indexCount = 0;
}; // RenderCommand_DrawCall

class RenderCommandBuffer : public NonCopyable {
public:
	RenderCommand_ClearFrameBuffers*	clearFrameBuffers()	{ return newCommand<RenderCommand_ClearFrameBuffers>();	}
	RenderCommand_SwapBuffers*			swapBuffers()		{ return newCommand<RenderCommand_SwapBuffers>();		}

	void drawMesh(const RenderMesh& mesh);

	Span<RenderCommand*>	commands() { return _commands; }

	template<class CMD>
	CMD* newCommand() {
		auto* buf = _allocator.allocate(sizeof(CMD));
		auto* cmd = new(buf) CMD();
		_commands.push_back(cmd);
		return cmd;
	}

	void reset() {
		for (auto* cmd : _commands) {
			cmd->~RenderCommand();
		}
		_commands.clear();
		_allocator.clear();
	}
private:
	Vector_<RenderCommand*, 64>	_commands;
	LinearAllocator _allocator;
}; // RenderCommandBuffer

} // namespace

