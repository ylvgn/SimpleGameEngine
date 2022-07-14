#pragma once

#include "Shader.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderContext;
class Material;
class MaterialPass;

struct MaterialPass_Stage : public NonCopyable {
	using Pass = MaterialPass;

	virtual ~MaterialPass_Stage() = default;

	MaterialPass_Stage(MaterialPass* pass, ShaderStage* shaderStage);
	const ShaderStageInfo* info() const { return _shaderStage->info(); }

protected:
	struct ConstBuffer {
		using DataType	= ShaderStageInfo::DataType;
		using Info		= ShaderStageInfo::ConstBuffer;
		using VarInfo	= ShaderStageInfo::Variable;

	};

	Pass* _pass = nullptr;
	ShaderStage* _shaderStage = nullptr;
	Vector_<ConstBuffer, 4>	_constBuffers;
};

struct MaterialPass_VertexStage : public MaterialPass_Stage {
	using Base = MaterialPass_Stage;
	MaterialPass_VertexStage(MaterialPass* pass, ShaderVertexStage* shaderStage)
		: Base(pass, shaderStage)
	{}
};

struct MaterialPass_PixelStage : public MaterialPass_Stage {
	using Base = MaterialPass_Stage;
	MaterialPass_PixelStage(MaterialPass* pass, ShaderPixelStage* shaderStage)
		: Base(pass, shaderStage)
	{}
};

class MaterialPass : public NonCopyable {
public:
	virtual ~MaterialPass() = default;

	using Pass			= MaterialPass;
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

protected:
	MaterialPass(Material* material, ShaderPass* shaderPass)
		: _material(material)
		, _shaderPass(shaderPass)
	{}

	Material*	_material		= nullptr;
	ShaderPass* _shaderPass		= nullptr;
	VertexStage* _vertexStage	= nullptr;
	PixelStage* _pixelStage		= nullptr;
};

class Material : public RefCountBase {
public:
	virtual ~Material() = default;

	using Pass			= MaterialPass;
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

	void setShader(Shader * shader);

protected:
	virtual void onSetShader() {}
	virtual UPtr<Pass> onCreatePass(ShaderPass* shaderPass) = 0;
	
	SPtr<Shader> _shader;
	Vector_<UPtr<Pass>, 2>	_passes;

}; // Material

} // namespace