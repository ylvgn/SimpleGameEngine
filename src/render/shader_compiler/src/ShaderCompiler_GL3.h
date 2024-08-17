#pragma once

#include <sge_render/backend/gl3/Render_GL3_Common.h>

namespace sge {

class ShaderCompiler_GL3 : public NonCopyable {
	using Util = GL3Util;
	using DataType = RenderDataType;
public:
	void compile(StrView outPath, ShaderStageMask shaderStage, StrView srcFilename, StrView entryFunc);
};

}