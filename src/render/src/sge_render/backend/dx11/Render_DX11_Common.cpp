#include "Render_DX11_Common.h"
#include "Renderer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

Renderer_DX11* DX11Util::renderer() {
	return Renderer_DX11::instance();
}

DX11_ID3DDevice* DX11Util::d3dDevice() {
	return renderer()->d3dDevice();
}

DX11_ID3DDeviceContext* DX11Util::d3dDeviceContext() {
	return renderer()->d3dDeviceContext();
}

VertexSemanticType DX11Util::parseDxSemanticName(StrView s) {
	VertexSemanticType v;

	if (s == "SV_POSITION") {
		return VertexSemanticType::POSITION;
	}

	if (!enumTryParse(v, s)) {
		throw SGE_ERROR("unknown VertexLayout_SemanticType {}", s);
	}
	return v;
}

const char* DX11Util::getDxSemanticName(VertexSemanticType v) {
	const char* s = enumStr(v);
	if (!s) {
		throw SGE_ERROR("unknown VertexLayout_SemanticType {}", v);
	}
	return s;
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11