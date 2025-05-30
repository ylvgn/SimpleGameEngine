#pragma once

#include <sge_render/shader/Material.h>

#if SGE_RENDER_HAS_DX11

#include "Shader_DX11.h"
#include "RenderGpuBuffer_DX11.h"

namespace sge {

class Material_DX11 : public Material {
	using Base = Material;
	using Util = DX11Util;

	sgeMaterial_InterfaceFunctions(DX11);

	class MyPass;

	#if 0
	#pragma mark ========= Material_DX11::MyVertexStage ============
	#endif
	class MyVertexStage : public Material::VertexStage {
		using Base = typename Material::VertexStage;
	public:
		using ShaderStage	= Shader_DX11::MyVertexStage;
		using ShaderPass	= Shader_DX11::MyPass;
		using Pass			= Material_DX11::MyPass;

		MyVertexStage(MyPass* pass, ShaderVertexStage* shaderStage) noexcept
			: Base(pass, shaderStage)
		{}

		ShaderStage* shaderStage() { return static_cast<ShaderStage*>(_shaderStage); }

		void bind  (RenderContext_DX11* ctx, RenderCommand_DrawCall& drawCall);
		void unbind(RenderContext_DX11* ctx, RenderCommand_DrawCall& drawCall);

		void _bindInputLayout(RenderContext_DX11* ctx, const VertexLayout* vertexLayout);

		void _dxSetConstBuffer(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DBuffer* d3dBuf) {
			dc->VSSetConstantBuffers(bindPoint, 1, &d3dBuf);
		}

		void _dxSetShaderResource(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DShaderResourceView* rv) {
			dc->VSSetShaderResources(bindPoint, 1, &rv);
		}

		void _dxSetSampler(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DSamplerState* ss) {
			dc->VSSetSamplers(bindPoint, 1, &ss);
		}

		VectorMap<const VertexLayout*, ComPtr<DX11_ID3DInputLayout>> _inputLayoutsMap;
	}; // MyVertexStage

	#if 0
	#pragma mark ========= Material_DX11::MyPixelStage ============
	#endif
	class MyPixelStage : public Material::PixelStage {
		using Base = typename Material::PixelStage;
	public:
		using ShaderStage	= Shader_DX11::MyPixelStage;
		using ShaderPass	= Shader_DX11::MyPass;
		using Pass			= Material_DX11::MyPass;

		MyPixelStage(MyPass* pass, ShaderPixelStage* shaderStage) noexcept
			: Base(pass, shaderStage)
		{}

		ShaderStage* shaderStage() { return static_cast<ShaderStage*>(_shaderStage); }

		void bind  (RenderContext_DX11* ctx, RenderCommand_DrawCall& drawCall);
		void unbind(RenderContext_DX11* ctx, RenderCommand_DrawCall& drawCall);

		void _dxSetConstBuffer(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DBuffer* d3dBuf) {
			dc->PSSetConstantBuffers(bindPoint, 1, &d3dBuf);
		}

		void _dxSetShaderResource(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DShaderResourceView* rv) {
			dc->PSSetShaderResources(bindPoint, 1, &rv);
		}

		void _dxSetSampler(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DSamplerState* ss) {
			dc->PSSetSamplers(bindPoint, 1, &ss);
		}
	}; // MyPixelStage


	#if 0
	#pragma mark ========= Material_DX11::MyComputeStage ============
	#endif
	class MyComputeStage : public Material::ComputeStage {
		using Base = typename Material::ComputeStage;
	public:
		using ShaderStage	= Shader_DX11::MyComputeStage;
		using ShaderPass	= Shader_DX11::MyPass;
		using Pass			= Material_DX11::MyPass;

		MyComputeStage(MyPass* pass, ShaderComputeStage* shaderStage) noexcept
			: Base(pass, shaderStage)
		{}

		ShaderStage* shaderStage() { return static_cast<ShaderStage*>(_shaderStage); }

		void bind  (RenderContext_DX11* ctx, RenderCommand_DrawCall& drawCall);
		void unbind(RenderContext_DX11* ctx, RenderCommand_DrawCall& drawCall);

		void _dxSetConstBuffer(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DBuffer* d3dBuf) {
			dc->CSSetConstantBuffers(bindPoint, 1, &d3dBuf);
		}

		void _dxSetShaderResource(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DShaderResourceView* rv) {
			dc->CSSetShaderResources(bindPoint, 1, &rv);
		}

		void _dxSetSampler(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DSamplerState* ss) {
			dc->CSSetSamplers(bindPoint, 1, &ss);
		}
	}; // MyComputeStage

	#if 0
	#pragma mark ========= Material_DX11::MyPass ============
	#endif
	class MyPass : public Material::Pass {
		using Base = typename Material::Pass;
	public:
		using MyShaderPass = typename Shader_DX11::MyPass;

		MyPass(Material_DX11* material, ShaderPass* shaderPass) noexcept;

		MyShaderPass* shaderPass() const { return static_cast<MyShaderPass*>(_shaderPass); }

		virtual void onBind  (RenderContext* ctx, RenderCommand_DrawCall& drawCall) final;
		virtual void onUnbind(RenderContext* ctx, RenderCommand_DrawCall& drawCall) final;

		void _bindRenderState(RenderContext_DX11* ctx);

		MyVertexStage	_myVertexStage;
		MyPixelStage	_myPixelStage;
		MyComputeStage  _myComputeStage;

		ComPtr<DX11_ID3DRasterizerState>	_rasterizerState;
		ComPtr<DX11_ID3DDepthStencilState>	_depthStencilState;
		ComPtr<DX11_ID3DBlendState>			_blendState;
	}; // MyPass

#if 0
#pragma mark ========= Material_DX11 ============
#endif
	using Pass			= MyPass;
	using VertexStage	= MyVertexStage;
	using PixelStage	= MyPixelStage;
	using ComputeStage  = MyComputeStage;

	Shader_DX11* shader() { return static_cast<Shader_DX11*>(_shader.ptr()); }

	template<class STAGE>
	static void s_bindStageHelper(RenderContext_DX11* ctx, STAGE* stage);

	template<class STAGE>
	static void s_unbindStageHelper(RenderContext_DX11* ctx, STAGE* stage);
}; // Material_DX11

} // namespace sge

#endif // SGE_RENDER_HAS_DX11