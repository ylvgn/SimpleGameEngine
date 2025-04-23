#pragma once

#include "Render_Common.h"
#include "RenderContext.h"
#include "shader/Material.h"
#include "textures/Texture.h"

namespace sge {

#define Renderer_ApiType_ENUM_LIST(E) \
	E(Unknown,) \
	E(DX11,) \
	E(OpenGL,) \
//----
SGE_ENUM_CLASS(Renderer_ApiType, u8)

class RenderContext;
struct RenderContext_CreateDesc;

class RenderGpuBuffer;
struct RenderGpuBuffer_CreateDesc;

class RenderBuiltInAssets;

class Renderer : public NonCopyable {
public:
	using BuiltInAssets = RenderBuiltInAssets;
	using ApiType = Renderer_ApiType;

	struct CreateDesc {
		CreateDesc() noexcept;

		ApiType apiType;
		bool	multithread : 1;
	};

	static Renderer* create(CreateDesc& desc);

	Renderer() noexcept;
	virtual ~Renderer();

	static Renderer* instance() { return s_instance; };

	const RenderAdapterInfo& adapterInfo() const { return _adapterInfo; };

	void	setVSync(bool b) { _vsync = b; }
	bool	vsync()			const { return _vsync; }

	ApiType	apiType()		const { return _apiType; }
	bool	multithread()	const { return _multithread; }

	SPtr<RenderContext>		createContext				(RenderContext_CreateDesc& desc)	{ return onCreateContext(desc); }
	SPtr<RenderGpuBuffer>	createGpuBuffer				(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc); }

	SPtr<Texture2D>			createTexture2D				(Texture2D_CreateDesc& desc)		{ return onCreateTexture2D(desc); }
	SPtr<Texture2D>			createTexture2D				(const Image& img, const SamplerState& samplerState = SamplerState());
	SPtr<Texture2D>			createTexture2DFromFile		(StrView filename, const SamplerState& samplerState = SamplerState());
	SPtr<Texture2D>			createSolidColorTexture2D	(const Color4b& color);

	SPtr<TextureCube>		createTextureCube			(TextureCube_CreateDesc& desc)		{ return onCreateTextureCube(desc); }
	SPtr<TextureCube>		createTextureCubeFromFiles	(Span<StrView> filenames, const SamplerState& samplerState = SamplerState());

	SPtr<Shader>			createShader				(StrView filename);
	SPtr<Material>			createMaterial				()									{ return onCreateMaterial(); };

	void onShaderDestory(Shader* shader);

	BuiltInAssets* builtInAssets() { return _builtInAssets; }

protected:
	virtual SPtr<RenderContext>		onCreateContext		(RenderContext_CreateDesc& desc) = 0;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer	(RenderGpuBuffer_CreateDesc& desc) = 0;
	virtual SPtr<Shader>			onCreateShader		(StrView filename) = 0;
	virtual SPtr<Material>			onCreateMaterial	() = 0;
	virtual SPtr<Texture2D>			onCreateTexture2D	(Texture2D_CreateDesc& desc) = 0;
	virtual SPtr<TextureCube>		onCreateTextureCube (TextureCube_CreateDesc& desc) = 0;

	static Renderer*		s_instance;

	ApiType					_apiType = ApiType::Unknown;
	StringMap<Shader*>		_shaders;
	RenderAdapterInfo		_adapterInfo;
	BuiltInAssets*			_builtInAssets = nullptr;

	bool					_multithread	: 1;
	bool					_vsync			: 1;
}; // Renderer

} // namespace sge


#define sgeRenderer_InterfaceFunctions(T) \
	virtual SPtr<RenderContext>		onCreateContext(RenderContext_CreateDesc& desc) override; \
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) override; \
	virtual SPtr<Shader>			onCreateShader(StrView filename) override; \
	virtual SPtr<Material>			onCreateMaterial() override; \
	virtual SPtr<Texture2D>			onCreateTexture2D(Texture2D_CreateDesc& desc) override; \
	virtual SPtr<TextureCube>		onCreateTextureCube(TextureCube_CreateDesc& desc) override; \
//-----

#define sgeRenderer_InterfaceFunctions_Impl(T) \
	SPtr<RenderContext>		Renderer_##T::onCreateContext(RenderContext_CreateDesc& desc) { SPtr<RenderContext> p = new RenderContext_##T(desc); p->onPostCreate(); return p; } \
	SPtr<RenderGpuBuffer>	Renderer_##T::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) { SPtr<RenderGpuBuffer> p = new RenderGpuBuffer_##T(); p->create(desc); return p; }; \
	SPtr<Shader>			Renderer_##T::onCreateShader(StrView filename) { SPtr<Shader> p = new Shader_##T(); p->loadFile(filename); return p; } \
	SPtr<Material>			Renderer_##T::onCreateMaterial() { return new Material_##T(); } \
	SPtr<Texture2D>			Renderer_##T::onCreateTexture2D(Texture2D_CreateDesc& desc) { return new Texture2D_##T(desc); } \
	SPtr<TextureCube>		Renderer_##T::onCreateTextureCube(TextureCube_CreateDesc& desc) { return new TextureCube_##T(desc); } \
//-----