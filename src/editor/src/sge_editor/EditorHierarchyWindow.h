#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorHierarchyWindow : public EditorWindow {
private:
	static constexpr const char* kDragDropEntityFlag = "DRAG_DROP_ENTITY";

	struct DrawRequest {
		RenderRequest&		req;
		EntitySelection&	sel;
		Scene&				scene;
	};

	virtual void onDraw(Scene& scene, RenderRequest& req) final;

	void _drawEntity(DrawRequest dr, CTransform* tran);
	void _drawEntityChildren(DrawRequest& dr, CTransform* tran);

	bool _active = true;
}; // EditorHierarchyWindow

} // namespace sge