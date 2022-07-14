#pragma once

#include "Render_Common.h"
#include "RenderContext.h"
#include "Shader/Material.h"

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

	static Renderer* create(CreateDesc& desc);

	Renderer();
	virtual ~Renderer();

	static Renderer* instance() { return s_instance; };
	const RenderAdapterInfo& adapterInfo() { return _adapterInfo; };

	bool vsync() const { return _vsync; }

	SPtr<RenderContext>		createContext(RenderContext_CreateDesc& desc)		{ return onCreateContext(desc); }
	SPtr<RenderContext>		createRenderContext(RenderContext_CreateDesc& desc) { return onCreateRenderContext(desc); }
	SPtr<RenderGpuBuffer>	createGpuBuffer(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc); }
	SPtr<Shader>			createShader(StrView filename);

	void onShaderDestory(Shader* shader);

protected:
	virtual SPtr<RenderContext>		onCreateContext(RenderContext_CreateDesc& desc) = 0;
	virtual SPtr<RenderContext>		onCreateRenderContext(RenderContext_CreateDesc& desc) = 0;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) = 0;
	virtual SPtr<Shader>			onCreateShader(StrView filename) = 0;

	StringMap<Shader*>	_shaders;

	static Renderer* s_instance;
	RenderAdapterInfo _adapterInfo;

	bool _vsync : 1;
}; // Renderer

} // namespace