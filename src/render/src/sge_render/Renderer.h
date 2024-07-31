#pragma once

#include "Render_Common.h"
#include "RenderContext.h"
#include "Shader/Material.h"
#include "textures/Texture.h"

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
		GL3,
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
	SPtr<RenderGpuBuffer>	createGpuBuffer(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc); }
	SPtr<Texture2D>			createTexture2D	(Texture2D_CreateDesc& desc)		{ return onCreateTexture2D(desc); }
	SPtr<Shader>			createShader(StrView filename);
	SPtr<Material>			createMaterial()									{ return onCreateMaterial(); };

	void onShaderDestory(Shader* shader);

	struct StockTextures {
		SPtr<Texture2D>	white;
		SPtr<Texture2D>	black;
		SPtr<Texture2D>	red;
		SPtr<Texture2D>	green;
		SPtr<Texture2D>	blue;
		SPtr<Texture2D>	magenta;
		SPtr<Texture2D>	error;
	};

	StockTextures stockTextures;

	SPtr<Texture2D>	createSolidColorTexture2D(const Color4b& color);

protected:
	virtual SPtr<RenderContext>		onCreateContext(RenderContext_CreateDesc& desc) = 0;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) = 0;
	virtual SPtr<Shader>			onCreateShader(StrView filename) = 0;
	virtual SPtr<Material>			onCreateMaterial() = 0;
	virtual SPtr<Texture2D>			onCreateTexture2D(Texture2D_CreateDesc& desc) = 0;

	StringMap<Shader*>	_shaders;

	static Renderer* s_instance;
	RenderAdapterInfo _adapterInfo;

	bool _vsync : 1;
	ApiType _apiType = ApiType::None;
}; // Renderer

} // namespace