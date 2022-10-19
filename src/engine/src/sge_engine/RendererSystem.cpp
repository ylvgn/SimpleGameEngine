#include "RendererSystem.h"

namespace sge {

RendererSystem* RendererSystem::s_instance = nullptr;

RendererSystem::RendererSystem() {
}

RendererSystem::~RendererSystem() {
	_renderers.clear();
}

RendererSystem* RendererSystem::createSytem() {
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

void RendererSystem::removeComponent(CRenderer* c) {
	for (auto it = _renderers.begin(); it != _renderers.end(); it++) {
		if (it->ptr() == c) {
			_renderers.erase(it);
			return;
		}
	}
}

void RendererSystem::removeComponent(Entity* e) {
	for (auto it = _renderers.begin(); it != _renderers.end(); it++) {
		if (it->ptr()->entity() == e) {
			_renderers.erase(it);
			return;
		}
	}
}

CRenderer* RendererSystem::getComponent(Entity* e) {
	for (auto& r : renderers()) {
		if (r->entity() == e) {
			return r.ptr();
		}
	}
	return nullptr;
}

void RendererSystem::render(RenderRequest& req) {
	for (auto& r : renderers()) {
		r->render(req);
	}
}

void RendererSystem::addComponent(CRenderer* c) {
	_renderers.emplace_back(c);
}

}

