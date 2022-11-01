#include "CRenderer.h"
#include <sge_engine/EngineContext.h>

namespace sge {

const TypeInfo* CRenderer::s_getType() {
	class Ti : public CRenderer::TI_Base {
	public:
		Ti() {

		}
	};
	static Ti ti;
	return &ti;
}

CRenderer::CRenderer() {
	EngineContext::rendererSystem()->add(this);
}

CRenderer::~CRenderer() {
	if (EngineContext::rendererSystem()) { // tmp
		EngineContext::rendererSystem()->remove(this);
	}
}

}