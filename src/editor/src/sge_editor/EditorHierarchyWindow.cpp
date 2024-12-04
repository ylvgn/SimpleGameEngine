#include "EditorHierarchyWindow.h"

namespace sge {

void EditorHierarchyWindow::_drawEntity(DrawRequest dr, CTransform* tran) {
	const static ImGuiTreeNodeFlags s_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	auto* entity = tran->entity();
	ImGuiTreeNodeFlags flags = tran->childCount() > 0
		? s_flags
		: s_flags | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

	if (dr.sel.has(entity->id())) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	EditorUI::TreeNode node(entity->name(), flags);

	if (EditorUI::IsItemClicked()) {
		if (ImGui::GetIO().KeyCtrl) {
			dr.sel.add(entity->id());
		} else {
			dr.sel.select(entity->id()); // but it can not multi drag drop..
		}
	}

	EditorUI::DragDropSource dragSrc;
	if (dragSrc.isOpen()) {
		ImGui::SetDragDropPayload(kDragDropEntityFlag, nullptr, 0, ImGuiCond_Once);
	}

	EditorUI::DragDropTarget dragTarget;
	if (const auto* payload = dragTarget.acceptDragDropPayload(kDragDropEntityFlag)) {
		for (const auto& id : dr.sel.entities()) {
			Entity* e = dr.scene.findEntityById(id);
			if (!e) continue;
			e->transform()->setParent(tran);
		}
	}
	if (!node.isOpen()) {
		return;
	}
	_drawEntityChildren(dr, tran);
}

void EditorHierarchyWindow::_drawEntityChildren(DrawRequest& dr, CTransform* tran) {
	for (auto& child : tran->children()) {
		if (!child) continue;
		_drawEntity(dr, child);
	}
}

void EditorHierarchyWindow::onDraw(Scene& scene, RenderRequest& req) {
	EditorUI::Window win("Hierarchy", &_active, ImGuiWindowFlags_MenuBar);

	auto* ed = EditorContext::instance();
	auto& sel = ed->entitySelection;

	DrawRequest dr{ req, sel, scene };
	auto* root = scene.rootEntity();
	if (root) {
		_drawEntityChildren(dr, root->transform());
	}
}

} // namespace sge