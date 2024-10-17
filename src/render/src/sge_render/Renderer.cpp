#include "Renderer.h"
#include "backend/dx11/Renderer_DX11.h"
#include "backend/opengl/Renderer_GL.h"

namespace sge {

Renderer* Renderer::s_instance = nullptr;

Renderer::CreateDesc::CreateDesc() 
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
		default: {
			SGE_ASSERT("error: not support render api");
		}break;
	}
	return p;
}

Renderer::Renderer() noexcept
	: _vsync(true)
{
	SGE_ASSERT(s_instance == nullptr);
	s_instance = this;
}

Renderer::~Renderer() {
	SGE_ASSERT(_shaders.size() == 0);
	SGE_ASSERT(s_instance == this);
	s_instance = nullptr;
}

SPtr<Shader> Renderer::createShader(StrView filename) {
	TempString tmpName = filename;
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

SPtr<Texture2D> Renderer::createSolidColorTexture2D(const Color4b& color) {
	int w = 4;
	int h = 4;
	Texture2D_CreateDesc texDesc;
	texDesc.colorType = ColorType::RGBAb;
	texDesc.mipmapCount = 1;
	texDesc.size.set(w, h);

	auto& image = texDesc.imageToUpload;
	image.create(Color4b::kColorType, w, h);

	for (int y = 0; y < w; y++) {
		auto row = image.row<Color4b>(y);
		for (int x = 0; x < h; x++) {
			row[x] = color;
		}
	}
	return createTexture2D(texDesc);
}

} // namespace sge