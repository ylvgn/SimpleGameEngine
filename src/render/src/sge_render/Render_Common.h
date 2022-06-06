#pragma once

#include <sge_core.h>

namespace sge {

struct RenderAdapterInfo {
	String	adapterName;
	i64		memorySize = 0;

	bool	multithread = false;
	bool	hasComputeShader = false;
	bool	shaderHasFloat64 = false;
	int		minThreadGroupSize = 1;
};

enum class RenderPrimitiveType {
	None,
	Points,
	Lines,
	Triangles,
};

enum class ShaderStage {
	None,
	Vertex,
	Pixel,
};

inline
const char* enumStr(ShaderStage v) {
	switch (v) {
#define E(T) case ShaderStage::T: return #T;
		E(None)
		E(Vertex)
		E(Pixel)
#undef E
	default: {
			SGE_ASSERT(false);
			return "";
		}
	}
}

SGE_FORMATTER_ENUM(ShaderStage)

} // namespace
