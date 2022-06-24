#pragma once
#include "Render_Common.h"

namespace sge {

// render context
class RenderContext;
struct RenderContext_CreateDesc;

// render gpu buffer
class RenderGpuBuffer;
struct RenderGpuBuffer_CreateDesc;

// abstract class
class Renderer : public NonCopyable {
public:

	enum class ApiType {
		None = 0,
		DX11,
		OpenGL,
	};

	struct CreateDesc {
		CreateDesc(); // declare constructor -> RenderContext::CreateDesc::CreateDesc()
		ApiType apiType;
		bool multithread : 1;
	};

	Renderer();
	virtual ~Renderer();

	static Renderer* instance() { return s_instance; };
	const RenderAdapterInfo& adapterInfo() { return _adapterInfo; };

	static Renderer*	create(CreateDesc& desc);
	RenderContext*		createRenderContext(RenderContext_CreateDesc& desc) { return onCreateRenderContext(desc); }
	RenderGpuBuffer*	createGpuBuffer(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc); }

protected:
	static Renderer* s_instance;
	RenderAdapterInfo _adapterInfo;

	virtual RenderContext*		onCreateRenderContext(RenderContext_CreateDesc& desc) = 0;
	virtual RenderGpuBuffer*	onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) = 0;
}; // Renderer

} // namespace