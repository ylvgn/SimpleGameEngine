#include "CRendererSystem.h"
#include "CRenderer.h"

namespace sge {

CRendererSystem* CRendererSystem::_instance = nullptr;

CRendererSystem::CRendererSystem() {
	SGE_ASSERT(_instance == nullptr);
	_instance = this;
}

CRendererSystem::~CRendererSystem() {
	SGE_ASSERT(_instance == this);
	_instance = nullptr;
}

void CRendererSystem::_internalAddComponent(CRenderer* c) {
	_rendererComponents.emplace_back(c);
}

void CRendererSystem::_internalRemoveComponent(CRenderer* c) {
	_rendererComponents.remove(c);
}

void CRendererSystem::render(RenderRequest& req) {
	for (auto* c : _rendererComponents) {
		c->_render(req);
	}
}

}

