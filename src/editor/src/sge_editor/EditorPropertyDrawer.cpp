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

void EditorPropertyDrawer_Vec3f::draw(DrawRequest& req) {
	bool isMixed = req.isMixedValue<Vec3f>();
	auto mv = makeScopedValue(&EditorUI::showMixedValue, isMixed);

	Vec3f oldV = req.getFirstObjectValue<Vec3f>();
	float newV[3] = { oldV.x, oldV.y, oldV.z };

	if (!EditorUI::DragFloat3(req.field->name, newV)) return;

	if (isMixed) return;

	for (auto& obj : req.objects) {
		req.field->setValue<Vec3f>(obj, Vec3f{newV[0], newV[1], newV[2]});
	}
}

void EditorPropertyDrawer_Quat4f::draw(DrawRequest& req) {
	bool isMixed = req.isMixedValue<Quat4f>();
	auto mv = makeScopedValue(&EditorUI::showMixedValue, isMixed);

	auto oldV = req.getFirstObjectValue<Quat4f>();
	float newV[4] = { oldV.x, oldV.y, oldV.z, oldV.w };

	if (!EditorUI::DragFloat4(req.field->name, newV)) return;

	if (isMixed) return;

	//const Vec3f& v3 = Vec3f{ newV[0], newV[1], newV[2] };
	//auto q = Quat4f::s_euler(v3); // not work
	for (auto& obj : req.objects) {
		req.field->setValue<Quat4f>(obj, Quat4f(newV[0], newV[1], newV[2], newV[3]));
	}
}

}