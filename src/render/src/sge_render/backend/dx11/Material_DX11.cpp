#include "Material_DX11.h"
#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

namespace sge {

Material_DX11::MyPass::MyPass(Material* material, ShaderPass* shaderPass)
	: Base(material, shaderPass)
	, _myVertexStage(this, shaderPass->vertexStage())
	, _myPixelStage(this, shaderPass->pixelStage())
{
	_vertexStage = &_myVertexStage;
	_pixelStage = &_myPixelStage;
}

} // namespace
