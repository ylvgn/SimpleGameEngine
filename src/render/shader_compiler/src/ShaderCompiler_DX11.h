#pragma once

#include <sge_render/backend/dx11/Render_DX11_Common.h>

namespace sge {

class ShaderCompiler_DX11 : public NonCopyable {
	using Util		= DX11Util;
	using DataType	= RenderDataType;
	using Profile	= ShaderStageProfile;
public:
	void compile(StrView outFilename
				, ShaderStageMask shaderStage
				, StrView profile
				, StrView srcFilename
				, StrView entryFunc
				, Vector<String>& include_dirs);

private:
	void _reflect(StrView outFilename, ByteSpan bytecode, ShaderStageMask stage, StrView profile);
	void _reflect_inputs		(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _reflect_constBuffers	(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _reflect_textures		(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _reflect_samplers		(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _reflect_storageBuffers(ShaderStageInfo& outInfo, DX11_ID3DShaderReflection* reflect, D3D11_SHADER_DESC& desc);
}; // ShaderCompiler_DX11

} // namespace sge