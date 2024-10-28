#pragma once

#include <sge_render/backend/opengl/Render_GL_Common.h>

#include "spirv_glsl.hpp"

namespace sge {

class ShaderCompiler_GL : public NonCopyable {
	using Util				= GLUtil;
	using DataType			= RenderDataType;
	using Profile			= ShaderStageProfile;

	using Compiler			= spirv_cross::CompilerGLSL;
	using CompilerOptions	= spirv_cross::CompilerGLSL::Options;
	using ShaderResources	= spirv_cross::ShaderResources;
	using SPIRType			= spirv_cross::SPIRType;

public:
	void compile(StrView outFilename
				, ShaderStageMask shaderStage
				, StrView profile
				, StrView srcFilename
				, StrView entryFunc
				, Vector<String>& include_dirs);

private:
	static void _convert(Compiler& comp, DataType& o, const SPIRType& i, u32 memberIndex = 0);

	static void _reflect(StrView outFilename, Compiler& comp, ShaderStageMask shaderStage, StrView profile);
	static void _reflect_inputs			(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);
	static void _reflect_constBuffers	(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);
	static void _reflect_textures		(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);
	static void _reflect_samplers		(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);

	static StrView _findLastNameWithoutUnderscore(StrView s);

	void _beforeCompileSPIRVToGLSL(Compiler& comp, ShaderStageMask shaderStage, StrView profile);

}; // ShaderCompiler_GL

} // namespace sge