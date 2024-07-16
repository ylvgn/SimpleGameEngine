#pragma once

#include "../MyCommon.h"

namespace sge {

enum class RenderPrimitiveType {
	None,
	Points,
	Lines,
	Triangles,
};

inline void g_throwIfError() {
	auto e = glGetError();
	if (e != GL_NO_ERROR) {
		auto* errStr = reinterpret_cast<const char*>(gluErrorString(e));
		TempString str(errStr);
		throw SGE_ERROR("glGetError = (0x{:X}) {}", static_cast<u32>(e), str);
	}
}

}