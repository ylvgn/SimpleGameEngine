#include "Material.h"
#include <sge_render/Renderer.h>

namespace sge {
#if 0
#pragma mark ========= MaterialPass_VertexStage ============
#endif
MaterialPass_VertexStage::MaterialPass_VertexStage(MaterialPass* pass, ShaderVertexStage* shaderStage)
	: Base(pass, shaderStage)
{
}

#if 0
#pragma mark ========= MaterialPass_PixelStage ============
#endif
MaterialPass_PixelStage::MaterialPass_PixelStage(MaterialPass* pass, ShaderPixelStage* shaderStage)
	: Base(pass, shaderStage)
{
}

#if 0
#pragma mark ========= MaterialPass_Stage ============
#endif
MaterialPass_Stage::MaterialPass_Stage(MaterialPass* pass, ShaderStage* shaderStage)
	: _pass(pass)
	, _shaderStage(shaderStage)
{
	auto* info = shaderStage->info();
	{
		auto cbCount = info->constBuffers.size();
		_constBuffers.resize(cbCount);

		for (size_t i = 0; i < cbCount; i++) {
			auto& cb = _constBuffers[i];
			cb.create(info->constBuffers[i]);
		}
	}

	{
		auto texCount = info->textures.size();
		_texParams.resize(texCount);

		for (size_t i = 0; i < texCount; i++) {
			auto& t = _texParams[i];
			t.create(info->textures[i]);
		}
	}
}

void MaterialPass_Stage::ConstBuffer::create(const Info& info) {
	_info = &info;
	_gpuDirty = true;

	cpuBuffer.resize(info.dataSize);

	RenderGpuBuffer::CreateDesc desc;
	desc.type = RenderGpuBufferType::Const;
	desc.bufferSize = info.dataSize;

	gpuBuffer = Renderer::instance()->createGpuBuffer(desc);
}

void MaterialPass_Stage::ConstBuffer::uploadToGpu() {
	if (!_gpuDirty) return;
	_gpuDirty = false;

	if (!gpuBuffer) return;
	gpuBuffer->uploadToGpu(cpuBuffer);
}

void MaterialPass_Stage::ConstBuffer::errorType() {
	throw SGE_ERROR("ConstBuffer setParam type mismatch");
}

Texture* MaterialPass_Stage::TexParam::getUpdatedTexture() {
	if (!_tex) {
		auto* renderer = Renderer::instance();
		switch (_info->dataType) {
			case DataType::Texture2D: return renderer->stockTextures.error; break;
			default: throw SGE_ERROR("unsupported texture type");
		}
	}

	// TODO update texture
	return _tex;
}


#if 0
#pragma mark ========= MaterialPass ============
#endif
MaterialPass::MaterialPass(Material* material, ShaderPass* shaderPass) noexcept
	: _material(material)
	, _shaderPass(shaderPass)
{
	SGE_ASSERT(material != nullptr);
	SGE_ASSERT(shaderPass != nullptr);
}


#if 0
#pragma mark ========= Material ============
#endif
void Material::setShader(Shader* shader) {
	if (_shader == shader)
		return;

	_shader = shader;
	_passes.clear();
	_passes.reserve(shader->passes().size());
	for (auto& shaderPass : shader->passes()) {
		UPtr<Pass> pass = onCreatePass(shaderPass.get());
		_passes.emplace_back(eastl::move(pass));
	}

	onSetShader();
}

} // namespace sge
