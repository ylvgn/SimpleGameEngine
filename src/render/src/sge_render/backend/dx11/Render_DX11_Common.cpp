#include "Render_DX11_Common.h"

namespace sge {

const char* DX11Util::getDxSemanticName(VertexSemanticType v) {
	const char* s = enumStr(v);
	if (!s) {
		throw SGE_ERROR("unknown VertexLayout_SemanticType {}", v);
	}
	return s;
}

} // namespace