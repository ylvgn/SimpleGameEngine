#include "Shader_DX11.h"
#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {
#if 0
#pragma mark ========= Shader_DX11 ============
#endif

sgeShader_InterfaceFunctions_Impl(DX11)

Shader_DX11::Shader_DX11(StrView filename)
	: Base(filename)
{
}

void Shader_DX11::_loadStageFile(StrView passPath, ShaderStageMask stageMask, Vector<u8>& outBytecode, ShaderStageInfo& outInfo) {
	auto* profile = Util::getDxStageProfile(stageMask);

	auto filename = Fmt("{}/{}.bin", passPath, profile);
	File::readFile(filename, outBytecode);

	filename += ".json";
	JsonUtil::readFile(filename, outInfo);
}

#if 0
#pragma mark ========= Shader_DX11::MyVertexStage ============
#endif
void Shader_DX11::MyVertexStage::load(MyPass* pass, StrView passPath, DX11_ID3DDevice* dev) {
	_loadStageFile(passPath, stageMask(), _bytecode, _info);
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
void Shader_DX11::MyPixelStage::load(MyPass* pass, StrView passPath, DX11_ID3DDevice* dev) {
	_loadStageFile(passPath, stageMask(), _bytecode, _info);
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
{
	_vertexStage = &_myVertexStage;
	 _pixelStage  = &_myPixelStage;
}

void Shader_DX11::MyPass::onInit() {
	auto* proj		= ProjectSettings::instance();
	auto* renderer	= Renderer_DX11::instance();
	auto* dev		= renderer->d3dDevice();

	TempString passPath;
	FmtTo(passPath, "{}/{}/dx11/pass{}", proj->importedPath(), shaderFilename(), _passIndex);

	if (_info->vsFunc.size()) { _myVertexStage.load(this, passPath, dev); }
	if (_info->psFunc.size()) {  _myPixelStage.load(this, passPath, dev); }
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11