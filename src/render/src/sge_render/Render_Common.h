#pragma once

#include <sge_core.h>

namespace sge {

#define RenderPrimitiveType_ENUM_LIST(E) \
	E(None,) \
	E(Points,) \
	E(Lines,) \
	E(Triangles,) \
//----
SGE_ENUM_CLASS(RenderPrimitiveType, u8)

struct RenderAdapterInfo {
	String	adapterName;
	i64		memorySize = 0;

	bool	multithread = false;
	bool	hasComputeShader = false;
	bool	shaderHasFloat64 = false;
	int		minThreadGroupSize = 1;
};

} // namespace sge
