#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorHierarchyWindow : public EditorWindow {
public:
	void draw(Scene& scene, RenderRequest& req);

private:
	void _drawinner(Scene& scene, RenderRequest& req, Entity* e);
	bool _active = true;
};

}