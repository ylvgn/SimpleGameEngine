#pragma once

#include <sge_render/command/RenderRequest.h>

namespace sge {

class CRenderer;

class RendererSystem : public NonCopyable {
public:
	static RendererSystem* instance() { return s_instance; }

	RendererSystem();
	~RendererSystem();

	static RendererSystem* createSystem();
	static void destroySystem();

	void add(CRenderer* c);
	void remove(CRenderer* c);

	void render(RenderRequest& req);

	Span<CRenderer*> renderers() { return _renderers; }

private:
	static RendererSystem* s_instance;
	Vector<CRenderer*>	_renderers;
};

}