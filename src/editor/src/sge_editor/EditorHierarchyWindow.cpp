#include "EditorHierarchyWindow.h"

namespace sge {

void EditorHierarchyWindow::draw(Scene& scene, RenderRequest& req) {
	EditorUI::Window win("Hierarchy", &_active, ImGuiWindowFlags_MenuBar);

	auto* ed = EditorContext::instance();
	auto& sel = ed->entitySelection;

	for (auto& e : scene.entities()) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;

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
	}
}

} // namespace