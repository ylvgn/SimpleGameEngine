#pragma once

#include <sge_render/shader/Shader.h>

#if SGE_RENDER_HAS_DX11

#include <sge_render/backend/dx11/Render_DX11_Common.h>

namespace sge {

class RenderContext_DX11;

class Shader_DX11 : public Shader {
	using Base		= Shader;
	using Util		= DX11Util;
	using Profile	= ShaderStageProfile;
	sgeShader_InterfaceFunctions(DX11);
public:

	class MyPass;

	#if 0
	#pragma mark ========= Shader_DX11::MyVertexStage ============
	#endif
	class MyVertexStage : public Shader::VertexStage {
		using Base = typename Shader::VertexStage;
	public:
		using Pass = MyPass;

		MyVertexStage(MyPass* pass) noexcept
			: Base(pass)
		{}

		void load(StrView passPath, DX11_ID3DDevice* dev);
		void bind(RenderContext_DX11* ctx);

		ByteSpan	bytecode()	const { return _bytecode; }
		MyPass*		pass()		const { return static_cast<MyPass*>(_pass); }

	private:
		ComPtr<DX11_ID3DVertexShader>	_shader;
		Vector<u8>						_bytecode;
	}; // MyVertexStage

	#if 0
	#pragma mark ========= Shader_DX11::MyPixelStage ============
	#endif
	class MyPixelStage : public Shader::PixelStage {
		using Base = typename Shader::PixelStage;
	public:
		using Pass = MyPass;

		MyPixelStage(MyPass* pass) noexcept
			: Base(pass)
		{}

		void load(StrView passPath, DX11_ID3DDevice* dev);
		void bind(RenderContext_DX11* ctx);

		ByteSpan	bytecode()	const { return _bytecode; }
		MyPass*		pass()		const { return static_cast<MyPass*>(_pass); }

	private:
		ComPtr<DX11_ID3DPixelShader>	_shader;
		Vector<u8>						_bytecode;
	}; // MyPixelStage

	#if 0
	#pragma mark ========= Shader_DX11::MyPass ============
	#endif
	class MyPass : public Shader::Pass {
		using Base = typename Shader::Pass;
	public:
		MyPass(Shader_DX11* shader, int passIndex) noexcept;

		Shader_DX11* shader() const { return static_cast<Shader_DX11*>(_shader); }

	private:
		MyPass(Shader* shader, int passIndex) = delete;

		virtual void onInit() final;

		MyVertexStage	_vertexStage;
		 MyPixelStage	_pixelStage;
	}; // MyPass

	#if 0
	#pragma mark ========= Shader_DX11 ============
	#endif
	using VertexStage	= MyVertexStage;
	using PixelStage	= MyPixelStage;
	using Pass			= MyPass;

	static void s_loadStageFile(StrView passPath, StrView profile, Vector<u8>& outBytecode, ShaderStageInfo& outInfo);

}; // Shader_DX11

} // namespace sge

#endif SGE_RENDER_HAS_DX11