#include "EditorInspectorWindow.h"

namespace sge {

void EditorInspectorWindow::draw(Scene& scene, RenderRequest& req) {
	EditorUI::Window win("Inspector", &_active, 0);

	auto* ed = EditorContext::instance();
	auto& sel = ed->entitySelection;

	_tempTypeList.clear();
	_tempTypeMap.clear();

	for (auto& id : sel.entities()) {
		const auto& e = scene.findEntityById(id);
		if (!e) continue;
		for (auto& c : e->components()) {
			if (!c) continue;
			_addTempList(c);
		}
		if (auto* c = RendererSystem::instance()->getComponent(e)) {
			_addTempList(c);
		}
	}

	for (auto& objectList : _tempTypeList) {
		drawComponent(req, *objectList);
	}
}

void EditorInspectorWindow::_addTempList(Component* c) {
	const auto* ti = c->getType();
	auto it = _tempTypeMap.find(ti);

	ObjectList* list;

	if (it == _tempTypeMap.end()) {
		list = &_tempTypeMap[ti];
		_tempTypeList.emplace_back(list);
	} else {
		list = &_tempTypeMap[ti];
	}

	list->emplace_back(c);
}

void EditorInspectorWindow::drawComponent(RenderRequest& req, ObjectList& list) {
	if (list.size() <= 0) return;
	auto* ed = EditorContext::instance();
	const auto* type = list[0]->getType();

	EditorUI::CollapsingHeader header(type->name);

	EditorPropertyDrawRequest dr;
	dr.objectType = type;
	dr.objects.clear();
	dr.objects.reserve(list.size());
	for (auto& o : list) {
		dr.objects.emplace_back(o);
	}

	for (const auto& f : type->fields()) {
		dr.field = &f;
		if (auto* drawer = ed->getPropertyDrawer(f.fieldType)) {
			drawer->draw(dr);
		}
	}

#if 1 // just for debug ------------------------------------------
	if (list.size() == 1 && type == TypeOf<CTransform>()) {
		ImGui::NewLine();

		auto* o = list[0];
		CTransform* t = sge_cast<CTransform>(o);

#define E(M, COL) { \
			float v[4] = { M[COL].x, M[COL].y, M[COL].z, M[COL].w }; \
			EditorUI::InputFloat4("COLOMN_" ## #COL, v); \
		} \
// ---------
		{
			ImGui::Text("Text LocalMatrix");
			auto& m = t->localMatrix();
			E(m, 0)
			E(m, 1)
			E(m, 2)
			E(m, 3)
			ImGui::NewLine();
		}

		{
			ImGui::Text("Text WorldMatrix");
			auto& m = t->worldMatrix();
			E(m, 0)
			E(m, 1)
			E(m, 2)
			E(m, 3)
			ImGui::NewLine();
		}
#undef E
#endif
	}
}

} // namespace
