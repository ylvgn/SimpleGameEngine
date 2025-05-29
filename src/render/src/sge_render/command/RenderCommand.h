#pragma once

#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/shader/Material.h>

namespace sge {

class RenderMesh;
class RenderSubMesh;

#define RenderCommandType_ENUM_LIST(E) \
	E(None,) \
	E(ClearFrameBuffers,) \
	E(SwapBuffers,) \
	E(DrawCall,) \
	E(SetScissorRect,) \
//----
SGE_ENUM_CLASS(RenderCommandType, u32)

#if 0
#pragma mark ========= RenderCommand ============
#endif
class RenderCommand : NonCopyable {
public:
	using Type = RenderCommandType;

	RenderCommand(Type type) noexcept
		: debugLoc(SGE_LOC)
		, _type(type)
	{}

	virtual ~RenderCommand() = default;

	SrcLoc debugLoc;

	Type type() const { return _type; }

private:
	Type _type = Type::None;
}; // RenderCommand


#if 0
#pragma mark ========= RenderCommand_ClearFrameBuffers ============
#endif
class RenderCommand_ClearFrameBuffers : public RenderCommand {
	using Base = RenderCommand;
	using This = RenderCommand_ClearFrameBuffers;
public:
	RenderCommand_ClearFrameBuffers() noexcept
		: Base(Type::ClearFrameBuffers)
	{}

	This& setColor(const Color4f& color_)	{ color		= color_;	return *this; }
	This& setDepth(float depth_)			{ depth		= depth_;	return *this; }
	This& setStencil(u8 stencil_)			{ stencil	= stencil_; return *this; }

	This& dontClearColor()		{ color.reset();	return *this; }
	This& dontClearDepth()		{ depth.reset();	return *this; }
	This& dontClearStencil()	{ stencil.reset();	return *this; }

	Opt<Color4f>	color	= Color4f(1,1,1,1);
	Opt<float>		depth	= 1;
	Opt<u8>			stencil = 0;
}; // RenderCommand_ClearFrameBuffers


#if 0
#pragma mark ========= RenderCommand_SwapBuffers ============
#endif
class RenderCommand_SwapBuffers : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_SwapBuffers() noexcept
		: Base(Type::SwapBuffers)
	{}

}; // RenderCommand_SwapBuffers


#if 0
#pragma mark ========= RenderCommand_DrawCall ============
#endif
class RenderCommand_DrawCall : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_DrawCall() noexcept
		: Base(Type::DrawCall)
	{}

	MaterialPass* getMaterialPass() {
		return material ? material->getPass(materialPassIndex) : nullptr;
	}

	bool isComputeCall() const { return threadGroupCount.has_value(); }

	RenderPrimitiveType		primitive = RenderPrimitiveType::None;

	const VertexLayout*		vertexLayout = nullptr;

	SPtr<RenderGpuBuffer>	vertexBuffer;
	RenderDataType			indexType = RenderDataType::UInt16;
	SPtr<RenderGpuBuffer>	indexBuffer;

	SPtr<Material>			material;
	int						materialPassIndex = 0;

	size_t					indexOffset		= 0;
	size_t					vertexOffset	= 0;
	size_t					vertexCount		= 0;
	size_t					indexCount		= 0;

	Opt<Vec3i>				threadGroupCount;
}; // RenderCommand_DrawCall

#if 0
#pragma mark ========= RenderCommand_SetScissorRect ============
#endif
class RenderCommand_SetScissorRect : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_SetScissorRect() noexcept
		: Base(Type::SetScissorRect)
	{}

	Rect2f rect {0};
}; // RenderCommand_SetScissorRect

#if 0
#pragma mark ========= RenderCommandBuffer ============
#endif
class RenderCommandBuffer : public NonCopyable {
public:

	void reset(RenderContext* ctx);

	Span<RenderCommand*> commands() { return _commands; }

	const Rect2f& scissorRect() const { return _scissorRect; }
	const Rect2f& viewport() const { return _viewport; }

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
		SGE_STATIC_ASSERT(TypeTraits::isBaseOf<RenderCommand, CMD>::value);
		auto* buf = _allocator.allocate(sizeof(CMD));
		auto* cmd = new(buf) CMD();
		_commands.emplace_back(cmd);
		return cmd;
	}

private:
	Vector<RenderCommand*, 64>	_commands;
	LinearAllocator				_allocator;
	Rect2f						_scissorRect {0};
	Rect2f						_viewport {0};
}; // RenderCommandBuffer

#if 0
#pragma mark ========= RenderScissorRectScope ============
#endif
class RenderScissorRectScope : public NonCopyable {
public:
	RenderScissorRectScope() = default;

	RenderScissorRectScope(RenderScissorRectScope && r) noexcept {
		_cmdBuf = r._cmdBuf;
		_rect = r._rect;
		r._cmdBuf = nullptr;
	}

	RenderScissorRectScope(RenderCommandBuffer* cmdBuf) noexcept {
		if (!cmdBuf) return;
		_rect = cmdBuf->scissorRect();
		_cmdBuf = cmdBuf;
	}

	~RenderScissorRectScope() noexcept { detach(); }

	void detach() {
		if (!_cmdBuf) return;
		_cmdBuf->setScissorRect(_rect);
		_cmdBuf = nullptr;
	}

private:
	RenderCommandBuffer* _cmdBuf = nullptr;
	Rect2f				 _rect{0};
}; // RenderScissorRectScope


} // namespace sge

