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

Renderer* Renderer::create(CreateDesc& desc)
{
	Renderer* ret = nullptr;
	switch (desc.apiType)
	{
		case Renderer::ApiType::DX11: {
			ret = new Renderer_DX11(desc);
		}break;

		case Renderer::ApiType::OpenGL: {
			SGE_ASSERT("error: not support OpenGL api");
		}break;

		default: {
			SGE_ASSERT("error: not support render api");
		}break;
	}
	return ret;
}

Renderer::Renderer() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = this;
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

} // namespace