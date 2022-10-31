#include "Renderer.h"

#include "backend/dx11/Renderer_DX11.h"

namespace sge {

Renderer* Renderer::s_instance = nullptr;

Renderer::CreateDesc::CreateDesc() {
#if SGE_OS_WINDOWS
	apiType = ApiType::DX11;
#else
	apiType = ApiType::None;
#endif

	multithread = false;
}

Renderer* Renderer::create(CreateDesc& desc) {
	Renderer* p = nullptr;
	switch (desc.apiType) {
		case Renderer::ApiType::DX11: {
			p = new Renderer_DX11(desc);
		}break;
		default: {
			SGE_ASSERT("error: not support render api");
		}break;
	}

	{
		p->stockTextures.white		= p->createSolidColorTexture2D(Color4b(255, 255, 255, 255));
		p->stockTextures.black		= p->createSolidColorTexture2D(Color4b(0,   0,   0,   255));
		p->stockTextures.red		= p->createSolidColorTexture2D(Color4b(255, 0,   0,   255));
		p->stockTextures.green		= p->createSolidColorTexture2D(Color4b(0,   255, 0,   255));
		p->stockTextures.blue		= p->createSolidColorTexture2D(Color4b(0,   0,   255, 255));
		p->stockTextures.magenta	= p->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
		p->stockTextures.error		= p->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
	}

	return p;
}

Renderer::Renderer() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = this;
	_vsync = true;
}

Renderer::~Renderer() {
	SGE_ASSERT(s_instance == this);
	s_instance = nullptr;
}

SPtr<Shader> Renderer::createShader(StrView filename)
{
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
		auto span = image.row<Color4b>(y);
		for (int x = 0; x < h; x++) {
			span[x] = color; // span[x] means row[x]
		}
	}
	return createTexture2D(texDesc);
}

} // namespace