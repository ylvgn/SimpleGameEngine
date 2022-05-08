#include "Renderer.h"

#include "backend/dx11/Renderer_DX11.h"

namespace sge {

Renderer* Renderer::_current = nullptr;

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
	SGE_ASSERT(_current == nullptr);
	_current = this;
}

Renderer::~Renderer() {
	SGE_ASSERT(_current == this);
	_current = nullptr;
}

} // namespace