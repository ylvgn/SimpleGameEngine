#include "EditorHierarchyWindow.h"

namespace sge {

static const char* kDragFlag = "MOVE_ENTITY";
void EditorHierarchyWindow::_drawinner(Scene& scene, RenderRequest& req, Entity* e) {

	auto* ed = EditorContext::instance();
	auto& sel = ed->entitySelection;

	const static ImGuiTreeNodeFlags s_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGuiTreeNodeFlags flags = e->children().size() > 0
		? s_flags
		: s_flags | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

	if (sel.has(e->id())) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	EditorUI::TreeNode node(e->name(), flags);

	if (EditorUI::IsItemClicked()) {
		if (ImGui::GetIO().KeyCtrl) {
			sel.add(e->id());
		} else {
			sel.select(e->id());
		}
	}

	EditorUI::DragDropSource dragSrc;
	if (dragSrc.isOpen()) {
		EntityId sendEntityId = e->id();
		ImGui::SetDragDropPayload(kDragFlag, &sendEntityId, sizeof(EntityId), ImGuiCond_Once);
	}

	EditorUI::DragDropTarget dragTarget;
	if (dragTarget.isOpen()) {
		if (const auto* payload = ImGui::AcceptDragDropPayload(kDragFlag)) {
			SGE_ASSERT(payload->DataSize == sizeof(EntityId));

			EntityId recvEntityId = *reinterpret_cast<EntityId*>(payload->Data);
			Entity* recvEntity = scene.findEntityById(recvEntityId);
			if (recvEntity) {
				//SGE_LOG("{} set parent {}", recvEntity->name(), e->name());
				recvEntity->setParent(e);
			}
		}
	}

	if (!node.isOpen()) {
		return;
	}

	for (auto& child : e->children()) {
		if (child) _drawinner(scene, req, child);
	}
}

void EditorHierarchyWindow::draw(Scene& scene, RenderRequest& req) {
	EditorUI::Window win("Hierarchy", &_active, ImGuiWindowFlags_MenuBar);

	for (auto& e : scene.entities()) {
		if (e->parent()) continue;
		_drawinner(scene, req, e);
	}
}

} // namespace