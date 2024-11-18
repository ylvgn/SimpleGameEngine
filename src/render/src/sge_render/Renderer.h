#pragma once

#include "Render_Common.h"
#include "RenderContext.h"
#include "shader/Material.h"
#include "textures/Texture.h"

namespace sge {

class RenderContext;
struct RenderContext_CreateDesc;

class RenderGpuBuffer;
struct RenderGpuBuffer_CreateDesc;

class Renderer : public NonCopyable {
public:

	enum class ApiType {
		None = 0,
		DX11,
		OpenGL,
	};

	struct CreateDesc {
		CreateDesc() noexcept;

		ApiType apiType;
		bool	multithread : 1;
	};

	static Renderer* create(CreateDesc& desc);

	Renderer() noexcept;
	virtual ~Renderer();

	static Renderer* instance()	{ return s_instance; };

	const RenderAdapterInfo& adapterInfo() const { return _adapterInfo; };

	bool vsync() const { return _vsync; }

	SPtr<RenderContext>		createContext		(RenderContext_CreateDesc& desc)	{ return onCreateContext(desc); }
	SPtr<RenderGpuBuffer>	createGpuBuffer		(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc); }
	SPtr<Texture2D>			createTexture2D		(Texture2D_CreateDesc& desc)		{ return onCreateTexture2D(desc); }
	SPtr<Shader>			createShader		(StrView filename);
	SPtr<Material>			createMaterial		()									{ return onCreateMaterial(); };

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
	virtual SPtr<RenderContext>		onCreateContext		(RenderContext_CreateDesc& desc) = 0;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer	(RenderGpuBuffer_CreateDesc& desc) = 0;
	virtual SPtr<Shader>			onCreateShader		(StrView filename) = 0;
	virtual SPtr<Material>			onCreateMaterial	() = 0;
	virtual SPtr<Texture2D>			onCreateTexture2D	(Texture2D_CreateDesc& desc) = 0;

	static Renderer* s_instance;

	StringMap<Shader*>	_shaders;
	RenderAdapterInfo	_adapterInfo;
	bool _vsync : 1;
}; // Renderer

} // namespace sge


#define sgeRenderer_InterfaceFunctions(T) \
	virtual SPtr<RenderContext>		onCreateContext(RenderContext_CreateDesc& desc) override; \
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) override; \
	virtual SPtr<Shader>			onCreateShader(StrView filename) override; \
	virtual SPtr<Material>			onCreateMaterial() override; \
	virtual SPtr<Texture2D>			onCreateTexture2D(Texture2D_CreateDesc& desc) override; \
//-----

#define sgeRenderer_InterfaceFunctions_Impl(T) \
	SPtr<RenderContext>		Renderer_##T::onCreateContext(RenderContext_CreateDesc& desc) { SPtr<RenderContext> p = new RenderContext_##T(desc); p->onPostCreate(); return p; } \
	SPtr<RenderGpuBuffer>	Renderer_##T::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) { SPtr<RenderGpuBuffer> p = new RenderGpuBuffer_##T(); p->create(desc); return p; }; \
	SPtr<Shader>			Renderer_##T::onCreateShader(StrView filename) { SPtr<Shader> p = new Shader_##T(); p->loadFile(filename); return p; } \
	SPtr<Material>			Renderer_##T::onCreateMaterial() { return new Material_##T(); } \
	SPtr<Texture2D>			Renderer_##T::onCreateTexture2D(Texture2D_CreateDesc& desc) { return new Texture2D_##T(desc); } \
//-----