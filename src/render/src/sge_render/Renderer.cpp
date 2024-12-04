#include "Renderer.h"
#include "backend/dx11/Renderer_DX11.h"
#include "backend/opengl/Renderer_GL.h"
#include "RenderBuiltInAssets.h"

namespace sge {

Renderer* Renderer::s_instance = nullptr;

Renderer::CreateDesc::CreateDesc() noexcept
	: multithread(false)
{
#if SGE_OS_WINDOWS
	apiType = ApiType::DX11;
#elif SGE_OS_LINUX
	apiType = ApiType::OpenGL;
#else
	apiType = ApiType::None;
#endif
}

Renderer* Renderer::create(CreateDesc& desc) {
	Renderer* p = nullptr;
	switch (desc.apiType) {
		case Renderer::ApiType::DX11: {
			p = new Renderer_DX11(desc);
		}break;
		case Renderer::ApiType::OpenGL: {
			p = new Renderer_GL(desc);
		}break;
		default: throw SGE_ERROR("unsupported renderer api");
	}
	return p;
}

Renderer::Renderer() noexcept
	: _vsync(true)
{
	SGE_ASSERT(s_instance == nullptr);
	s_instance = this;

	_builtInAssets = new BuiltInAssets();
}

Renderer::~Renderer() {
	SGE_ASSERT(_shaders.size() == 0);
	SGE_ASSERT(s_instance == this);

	if (_builtInAssets) {
		sge_delete(_builtInAssets);
		_builtInAssets = nullptr;
	}

	s_instance = nullptr;
}

SPtr<Texture2D> Renderer::createTexture2D(const Image& image, const SamplerState& samplerState) {
	Texture2D_CreateDesc texDesc;
	texDesc.samplerState = samplerState;
	texDesc.colorType	 = image.colorType();
	texDesc.size		 = image.size();

	Texture2D::UploadRequest uploadRequest;
	texDesc.uploadRequest = &uploadRequest;
	uploadRequest.assign(image);

	return createTexture2D(texDesc);
}

SPtr<Texture2D> Renderer::createTexture2DFromFile(StrView filename, const SamplerState& samplerState) {
	Image image;
	image.loadFile(filename);
	return createTexture2D(image, samplerState);
}

SPtr<Texture2D> Renderer::createSolidColorTexture2D(const Color4b& color) {
	using SRC = TypeTraits::removeConstRef<decltype(color)>::Type;

	int w = 4;
	int h = 4;

	Image image;
	image.create(color.kColorType, w, h);

	for (int y = 0; y < w; y++) {
		auto row = image.row_noCheck<SRC>(y);
		for (int x = 0; x < h; x++) {
			row[x] = color;
		}
	}
	return createTexture2D(image);
}

SPtr<Shader> Renderer::createShader(StrView filename) {
	TempString tmpName(filename);
	auto it = _shaders.find(tmpName.c_str());
	if (it != _shaders.end()) {
		return it->second;
	}

	auto s = onCreateShader(tmpName);
	_shaders[tmpName.c_str()] = s.ptr();
	return s;
}

void Renderer::onShaderDestory(Shader* shader) {
	_shaders.erase(shader->filename().c_str());
}

} // namespace sge