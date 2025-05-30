#include "Shader_DX11.h"
#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {
#if 0
#pragma mark ========= Shader_DX11::MyVertexStage ============
#endif
void Shader_DX11::MyVertexStage::load(StrView passPath, DX11_ID3DDevice* dev) {
	s_loadStageFile(passPath, Profile::DX11_VS, _bytecode, _info);
	auto hr = dev->CreateVertexShader(_bytecode.data(), _bytecode.size(), nullptr, _shader.ptrForInit());
	Util::throwIfError(hr);
}

void Shader_DX11::MyVertexStage::bind(RenderContext_DX11* ctx) {
	auto* dc = ctx->renderer()->d3dDeviceContext();
	if (!_shader) throw SGE_ERROR("dx shader is null");
	dc->VSSetShader(_shader, nullptr, 0);
}

void Shader_DX11::MyVertexStage::unbind(RenderContext_DX11* ctx) {
}

#if 0
#pragma mark ========= Shader_DX11::MyPixelStage ============
#endif
void Shader_DX11::MyPixelStage::load(StrView passPath, DX11_ID3DDevice* dev) {
	s_loadStageFile(passPath, Profile::DX11_PS, _bytecode, _info);
	auto hr = dev->CreatePixelShader(_bytecode.data(), _bytecode.size(), nullptr, _shader.ptrForInit());
	Util::throwIfError(hr);
}

void Shader_DX11::MyPixelStage::bind(RenderContext_DX11* ctx) {
	auto* dc = ctx->renderer()->d3dDeviceContext();
	if (!_shader) throw SGE_ERROR("dx shader is null");
	dc->PSSetShader(_shader, nullptr, 0);
}

void Shader_DX11::MyPixelStage::unbind(RenderContext_DX11* ctx) {
}

#if 0
#pragma mark ========= Shader_DX11::MyComputeStage ============
#endif
void Shader_DX11::MyComputeStage::load(StrView passPath, DX11_ID3DDevice* dev) {
	s_loadStageFile(passPath, Profile::DX11_CS, _bytecode, _info);
	auto hr = dev->CreateComputeShader(_bytecode.data(), _bytecode.size(), nullptr, _shader.ptrForInit());
	Util::throwIfError(hr);
}

void Shader_DX11::MyComputeStage::bind(RenderContext_DX11* ctx) {
	auto* dc = ctx->renderer()->d3dDeviceContext();
	if (!_shader) throw SGE_ERROR("dx shader is null");
	dc->CSSetShader(_shader, nullptr, 0);
}

void Shader_DX11::MyComputeStage::unbind(RenderContext_DX11* ctx) {
}

#if 0
#pragma mark ========= Shader_DX11::MyPass ============
#endif
Shader_DX11::MyPass::MyPass(Shader_DX11* shader, int passIndex) noexcept
	: Base(shader, passIndex)
	, _myVertexStage(this)
	, _myPixelStage(this)
	, _myComputeStage(this)
{
	_vertexStage	= &_myVertexStage;
	_pixelStage		= &_myPixelStage;
	_computeStage	= &_myComputeStage;
}

void Shader_DX11::MyPass::onInit() {
	auto* proj		= ProjectSettings::instance();
	auto* renderer	= Renderer_DX11::instance();
	auto* dev		= renderer->d3dDevice();

	TempString passPath;
	FmtTo(passPath, "{}/{}/dx11/pass{}", proj->importedPath(), shaderFilename(), _passIndex);

	if (hasVS()) { _myVertexStage.load(passPath, dev); }
	if (hasPS()) { _myPixelStage.load(passPath, dev); }
	if (hasCS()) { _myComputeStage.load(passPath, dev); }
}

#if 0
#pragma mark ========= Shader_DX11 ============
#endif

sgeShader_InterfaceFunctions_Impl(DX11)

void Shader_DX11::s_loadStageFile(StrView passPath, StrView profile, Vector<u8>& outBytecode, ShaderStageInfo& outInfo) {
	auto filename = Fmt("{}/{}.bin", passPath, profile);
	File::readFile(filename, outBytecode);

	filename += ".json";
	JsonUtil::readFile(filename, outInfo);
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11