#include "CRenderer.h"
#include "../RendererSystem.h"

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
	RendererSystem::instance()->add(this);
}

CRenderer::~CRenderer() {
	RendererSystem::instance()->remove(this);
}

}