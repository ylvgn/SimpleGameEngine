#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorHierarchyWindow : public EditorWindow {
public:
	void draw(Scene& scene, RenderRequest& req);

private:

	struct DrawRequest {
		RenderRequest&		req;
		EntitySelection&	sel;
		Scene&				scene;
	};

	static constexpr const char* kDragDropEntityFlag = "DRAG_DROP_ENTITY";

	void drawEntity(DrawRequest dr, CTransform* tran);
	void drawEntityChildren(DrawRequest& dr, CTransform* tran);

	bool _active = true;
};

}