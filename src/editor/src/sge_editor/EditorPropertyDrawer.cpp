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
		if (!f.fieldType) continue;
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

	Vec3f delta{ newV[0] - oldV.x, newV[1] - oldV.y, newV[2] - oldV.z };
	for (auto& obj : req.objects) {
		Vec3f v = req.field->getValue<Vec3f>(obj);
		Vec3f nv = v + delta;
		req.field->setValue<Vec3f>(obj, nv);
	}
}

void EditorPropertyDrawer_Quat4f::draw(DrawRequest& req) {
	bool isMixed = req.isMixedValue<Quat4f>();
	auto mv = makeScopedValue(&EditorUI::showMixedValue, isMixed);

	auto oldQuat = req.getFirstObjectValue<Quat4f>();
	auto oldV = oldQuat.euler();
	float newV[3] = { Math::degrees(oldV.x), Math::degrees(oldV.y), Math::degrees(oldV.z) };

	if (!EditorUI::DragFloat3(req.field->name, newV)) return;

	if (isMixed) return;

	Quat4f newQuat = Quat4f::s_euler({ Math::radians(newV[0]), Math::radians(newV[1]), Math::radians(newV[2]) });
	Quat4f delta { newQuat.x - oldQuat.x, newQuat.y - oldQuat.y, newQuat.z - oldQuat.z, newQuat.w - oldQuat.w };
	for (auto& obj : req.objects) {
		Quat4f v = req.field->getValue<Quat4f>(obj);
		Quat4f nv = v + delta;
		req.field->setValue<Quat4f>(obj, nv);
	}
}

}