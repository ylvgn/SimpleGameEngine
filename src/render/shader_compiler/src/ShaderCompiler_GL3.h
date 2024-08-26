#pragma once

#include <sge_render/backend/gl3/Render_GL3_Common.h>

#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"

namespace sge {

class ShaderCompiler_GL3 : public NonCopyable {
	using Util				= GL3Util;
	using DataType			= RenderDataType;
	using Compiler			= spirv_cross::CompilerGLSL;
	using CompilerOptions	= spirv_cross::CompilerGLSL::Options;
	using ShaderResources	= spirv_cross::ShaderResources;
	using SPIRType			= spirv_cross::SPIRType;
public:
	void compile(StrView outPath, ShaderStageMask shaderStage, StrView srcFilename, StrView entryFunc);

private:
	static void _convert(Compiler& comp, DataType& o, const SPIRType& i, u32 memberIndex = 0);

	static void _reflect(StrView outFilename, Compiler& comp, ShaderStageMask shaderStage, StrView profile);
	static void _reflect_inputs(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);
	static void _reflect_constBuffers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);
	static void _reflect_textures(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);
	static void _reflect_samplers(ShaderStageInfo& outInfo, Compiler& comp, const ShaderResources& resources);
};

}