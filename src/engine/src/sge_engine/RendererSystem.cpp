#include "RendererSystem.h"
#include "components/CRenderer.h"

namespace sge {

RendererSystem* RendererSystem::s_instance = nullptr;

RendererSystem::RendererSystem() {
}

RendererSystem::~RendererSystem() {
	_renderers.clear();
}

RendererSystem* RendererSystem::createSystem() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = new RendererSystem();
	return s_instance;
}

void RendererSystem::destroySystem() {
	if (s_instance) {
		s_instance->~RendererSystem();
		s_instance = nullptr;
	}
}

void RendererSystem::remove(CRenderer* c) {
	int index = -1;
	for (int i = 0; i < _renderers.size(); ++i) {
		if (index > 0) {
			_renderers[index++] = _renderers[i];
			continue;
		}
		if (_renderers[i] == c) {
			index = i;
		}
	}
	if (index > 0) {
		_renderers.pop_back();
	}
}

void RendererSystem::render(RenderRequest& req) {
	for (auto& r : renderers()) {
		r->render(req);
	}
}

void RendererSystem::add(CRenderer* c) {
	_renderers.emplace_back(c);
}

}

