#include "EditorPropertyDrawer.h"
#include "EditorUI.h"

namespace sge {

void EditorPropertyDrawer_struct::draw(DrawRequest& req) {
	auto* ed = EditorContext::instance();
	auto* type = req.field->fieldType;

	EditorUI::CollapsingHeader header(req.field->name);
	EditorUI::PushID pushId(req.field);

	EditorPropertyDrawRequest dr;
	dr.objectType = type;
	dr.objects.clear();
	dr.objects.reserve(req.objects.size());
	for (auto& o : req.objects) {
		dr.objects.emplace_back(req.field->getValuePtr(o));
	}

	for (const auto& f : type->fields()) {
		dr.field = &f;
		if (auto* drawer = ed->getPropertyDrawer(f.fieldType)) {
			drawer->draw(dr);
		}
	}
}

void EditorPropertyDrawer_float::draw(DrawRequest& req) {
	bool isMixed = req.isMixedValue<float>();
	auto mv = makeScopedValue(&EditorUI::showMixedValue, isMixed);

	float oldV = req.getFirstObjectValue<float>();
	float newV = oldV;

	if (!EditorUI::DragFloat(req.field->name, &newV)) return;

	if (isMixed) return;

	float delta = newV - oldV;
	for (auto& obj : req.objects) {
		float v = req.field->getValue<float>(obj);
		float nv = v + delta;
		req.field->setValue<float>(obj, nv);
	}
}


}