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

	if (ti->isContainer()) {
		// TODO
	}

	if (ti == TypeManager::instance()->getType("u8") || ti == TypeManager::instance()->getType("i8")
		|| ti == TypeManager::instance()->getType("u16") || ti == TypeManager::instance()->getType("i16")
		|| ti == TypeManager::instance()->getType("u32") || ti == TypeManager::instance()->getType("i32")
		|| ti == TypeManager::instance()->getType("u64") || ti == TypeManager::instance()->getType("i64"))
	{
		ImGui::InputInt(fi->name, reinterpret_cast<i32*>(p));
	}
	else if (ti == TypeManager::instance()->getType("f32"))
	{
		ImGui::InputFloat(fi->name, reinterpret_cast<f32*>(p));
	}
	else if (ti == TypeManager::instance()->getType("f64")
		|| ti == TypeManager::instance()->getType("f128"))
	{
		ImGui::InputDouble(fi->name, reinterpret_cast<f64*>(p));
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
