#include "Shader_DX11.h"
#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {
void Shader_DX11::MyVertexStage::load(StrView passPath, DX11_ID3DDevice* dev) {
	s_loadStageFile(passPath, stageMask(), _bytecode, _info);
	auto hr = dev->CreateVertexShader(_bytecode.data(), _bytecode.size(), nullptr, _shader.ptrForInit());
	Util::throwIfError(hr);
}

void Shader_DX11::MyVertexStage::bind(RenderContext_DX11* ctx) {
	auto* dc = ctx->renderer()->d3dDeviceContext();
	if (!_shader) throw SGE_ERROR("dx shader is null");
	dc->VSSetShader(_shader, 0, 0);
}

#if 0
#pragma mark ========= Shader_DX11::MyPixelStage ============
#endif
void Shader_DX11::MyPixelStage::load(StrView passPath, DX11_ID3DDevice* dev) {
	s_loadStageFile(passPath, stageMask(), _bytecode, _info);
	auto hr = dev->CreatePixelShader(_bytecode.data(), _bytecode.size(), nullptr, _shader.ptrForInit());
	Util::throwIfError(hr);
}

void Shader_DX11::MyPixelStage::bind(RenderContext_DX11* ctx) {
	auto* dc = ctx->renderer()->d3dDeviceContext();
	if (!_shader) throw SGE_ERROR("dx shader is null");
	dc->PSSetShader(_shader, 0, 0);
}

#if 0
#pragma mark ========= Shader_DX11::MyPass ============
#endif
Shader_DX11::MyPass::MyPass(Shader_DX11* shader, int passIndex) noexcept
	: Base(shader, passIndex)
	, _vertexStage(this)
	, _pixelStage(this)
{
	Base::_vertexStage = &_vertexStage;
	 Base::_pixelStage  = &_pixelStage;
}

void Shader_DX11::MyPass::onInit() {
	auto* proj		= ProjectSettings::instance();
	auto* renderer	= Renderer_DX11::instance();
	auto* dev		= renderer->d3dDevice();

	TempString passPath;
	FmtTo(passPath, "{}/{}/dx11/pass{}", proj->importedPath(), shaderFilename(), _passIndex);

	if (_info->vsFunc.size()) { _vertexStage.load(passPath, dev); }
	if (_info->psFunc.size()) {  _pixelStage.load(passPath, dev); }
}

#if 0
#pragma mark ========= Shader_DX11 ============
#endif

sgeShader_InterfaceFunctions_Impl(DX11)

void Shader_DX11::s_loadStageFile(StrView passPath, ShaderStageMask stageMask, Vector<u8>& outBytecode, ShaderStageInfo& outInfo) {
	auto* profile = Util::getDxStageProfile(stageMask);

	auto filename = Fmt("{}/{}.bin", passPath, profile);
	File::readFile(filename, outBytecode);

	filename += ".json";
	JsonUtil::readFile(filename, outInfo);
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11