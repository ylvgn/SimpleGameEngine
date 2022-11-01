#pragma once

#include "CSystem.h"
#include <sge_render/command/RenderRequest.h>

namespace sge {

class CRenderer;

class RendererSystem : public CSystem {
public:

	~RendererSystem();

	void add(CRenderer* c);
	void remove(CRenderer* c);

	void render(RenderRequest& req, const Math::Camera3f& camera); // test

	Span<CRenderer*> renderers() { return _renderers; }

private:
	Vector<CRenderer*>	_renderers;
};

}