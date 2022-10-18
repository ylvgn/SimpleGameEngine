#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorHierarchyWindow : public EditorWindow {
public:
	void draw(Scene& scene, RenderRequest& req);

private:

	static constexpr const char* kDragDropEntityFlag = "DRAG_DROP_ENTITY";

	void drawEntity(Scene& scene, RenderRequest& req, Entity* e);
	bool _active = true;
};

}