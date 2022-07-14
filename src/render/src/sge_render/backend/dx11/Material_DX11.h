#pragma once

#include <sge_render/shader/Material.h>
#include "Shader_DX11.h"
#include "RenderGpuBuffer_DX11.h"

namespace sge {

class Material_DX11 : public Material {
public:

private:
	struct MyVertexStage : public VertexStage {
		using Base = VertexStage;
		using MyShaderStage = Shader_DX11::MyVertexStage;
		MyVertexStage(MaterialPass* pass, ShaderVertexStage* shaderStage)
			: Base(pass, shaderStage)
		{}
	};

	struct MyPixelStage : public PixelStage {
		using Base = PixelStage;
		using MyShaderStage = Shader_DX11::MyPixelStage;

		MyPixelStage(MaterialPass* pass, ShaderPixelStage* shaderStage)
			: Base(pass, shaderStage)
		{}
	};

	struct MyPass : public Pass {
		using Base = Pass;
		MyPass(Material* material, ShaderPass* shaderPass);

		MyVertexStage _myVertexStage;
		MyPixelStage  _myPixelStage;
	};

	virtual UPtr<Pass> onCreatePass(ShaderPass* shaderPass) override {
		return UPtr<Pass>(new MyPass(this, shaderPass));
	}

}; // Material_DX11

} // namespace