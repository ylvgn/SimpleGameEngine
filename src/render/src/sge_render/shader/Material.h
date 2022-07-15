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

		Vector<u8>				cpuBuffer;
		SPtr<RenderGpuBuffer>	gpuBuffer;

		void create(const Info& info);

		void uploadToGpu();

		const Info* info() const { return _info; }
	private:
		void errorType();

		const Info*		_info = nullptr;
		bool			_gpuDirty = false;
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

	void bind(RenderContext* ctx, const VertexLayout* vertexLayout) { onBind(ctx, vertexLayout); }

protected:
	MaterialPass(Material* material, ShaderPass* shaderPass)
		: _material(material)
		, _shaderPass(shaderPass)
	{}

	virtual void onBind(RenderContext* ctx, const VertexLayout* vertexLayout) = 0;

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
	Span<UPtr<Pass>>	passes() { return _passes; }

	Pass* getPass(size_t index) {
		if (index >= _passes.size()) {
			SGE_ASSERT(false);
			return nullptr;
		}
		return _passes[index].get();
	}

protected:
	virtual void onSetShader() {}
	virtual UPtr<Pass> onCreatePass(ShaderPass* shaderPass) = 0;
	
	SPtr<Shader> _shader;
	Vector_<UPtr<Pass>, 2>	_passes;

}; // Material

} // namespace