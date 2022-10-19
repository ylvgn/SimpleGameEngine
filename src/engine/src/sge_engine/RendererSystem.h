#pragma once

#include "components/CRenderer.h"

namespace sge {

class RendererSystem : public NonCopyable {
public:
	static RendererSystem* instance() { return s_instance; }

	RendererSystem();
	~RendererSystem();

	static RendererSystem* createSytem();
	static void destroySystem();

	void addComponent(CRenderer* c);

	void removeComponent(CRenderer* c);
	void removeComponent(Entity* e);

	CRenderer* getComponent(Entity* e);

	void render(RenderRequest& req);

	Span<SPtr<CRenderer>> renderers() { return _renderers; }

private:
	static RendererSystem* s_instance;
	Vector<SPtr<CRenderer>>	_renderers;
};

}