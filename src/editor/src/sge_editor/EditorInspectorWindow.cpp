#include "EditorInspectorWindow.h"

namespace sge {

void test(u8* p, const TypeInfo* ti, const FieldInfo* fi = nullptr) {
	if (ti->isStruct() || ti->isObject()) {
		ImGui::Text(ti->name);
		for (const auto& field : ti->fieldArray) {
			test(p + field.offset, field.fieldInfo, &field);
		}
		return;
	}

	if (ti->isContainer()) return; // TODO

	TempString s;
	FmtTo(s, "{}: {}", fi->name, reinterpret_cast<intptr_t>(p));

	if (ti == TypeManager::instance()->getType("int8_t")
		|| ti == TypeManager::instance()->getType("int16_t")
		|| ti == TypeManager::instance()->getType("int32_t")
		|| ti == TypeManager::instance()->getType("int64_t"))
	{
		ImGui::DragInt(s.c_str(), reinterpret_cast<int*>(p));
	} else if (ti == TypeManager::instance()->getType("float")) {
		ImGui::DragFloat(s.c_str(), reinterpret_cast<float*>(p));
	}
}

void EditorInspectorWindow::draw(Scene& scene, RenderRequest& req) {
	EditorUI::Window win("Inspector", &_active, 0);

	auto* ed = EditorContext::instance();
	auto& sel = ed->entitySelection;

	for (auto& id : sel.entities()) {
		const auto& e = scene.findEntityById(id);
		ImGui::Text("%s", e->name().data());
		for (auto& c : e->components()) {
			const auto* ti = c->getType();
			test(reinterpret_cast<u8*>(c.ptr()), ti);
		}
		ImGui::NewLine();
	}
}

} // namespace
