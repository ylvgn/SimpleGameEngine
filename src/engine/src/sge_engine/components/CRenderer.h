#pragma once

#include "Component.h"
#include <sge_render.h>

namespace sge {

class CRenderer : public Component {
	SGE_OBJECT_TYPE(CRenderer, Component)
public:
	void render(RenderRequest& req) { onRender(req); }
protected:
	virtual void onRender(RenderRequest& req) = 0;
};

template<> inline
Object* TypeCreator<CRenderer>() {
	return nullptr;
}

}