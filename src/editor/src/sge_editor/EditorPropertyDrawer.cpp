#include "EditorPropertyDrawer.h"
#include "EditorUI.h"

namespace sge {

void EditorPropertyDrawer_struct::draw(DrawRequest& parentreq) {
	auto* ed = EditorContext::instance();
	auto* type = parentreq.field->fieldType;

	EditorUI::CollapsingHeader header(parentreq.field->name, ImGuiTreeNodeFlags_DefaultOpen);
	EditorUI::PushID pushId(parentreq.field);

	DrawRequest dr;
	parentreq.createChildRequest(dr);

	for (const auto& f : type->fields()) {
		if (!f.fieldType) continue;
		if (auto* drawer = ed->getPropertyDrawer(f.fieldType)) {
			dr.field = &f;
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

	for (auto& obj : req.objects) {
		req.field->setValue<float>(obj, newV);
	}
}

void EditorPropertyDrawer_Vec3f::draw(DrawRequest& req) {
	EditorUI::CollapsingHeader header(req.field->name, ImGuiTreeNodeFlags_DefaultOpen);
	EditorUI::PushID pushId(req.field);

	auto value = req.getFirstObjectValue<Vec3f>();

	bool isMixedX = false;
	bool isMixedY = false;
	bool isMixedZ = false;

	if (req.objects.size() > 1) {
		for (auto& obj : req.objects) {
			const auto& tmp = req.field->getValue<Vec3f>(obj);
			if (tmp.x != value.x) isMixedX = true;
			if (tmp.y != value.y) isMixedY = true;
			if (tmp.z != value.z) isMixedZ = true;
		}
	}

	struct Helper {
		static bool drawField(const char* name, float& newV, bool& isMixed) {
			auto mv = makeScopedValue(&EditorUI::showMixedValue, isMixed);
			return EditorUI::DragFloat(name, &newV);
		}
	};

	Vec3f newV(value);

	bool updatedX = Helper::drawField("x", newV.x, isMixedX);
	bool updatedY = Helper::drawField("y", newV.y, isMixedY);
	bool updatedZ = Helper::drawField("z", newV.z, isMixedZ);
	if (!(updatedX || updatedY || updatedZ)) return;

	auto delta = newV - value;
	for (auto& obj : req.objects) {
		auto v = req.field->getValue<Vec3f>(obj);
		req.field->setValue<Vec3f>(obj, v + delta);
	}
}

void EditorPropertyDrawer_Quat4f::draw(DrawRequest& req) {
	EditorUI::CollapsingHeader header(req.field->name, ImGuiTreeNodeFlags_DefaultOpen);
	EditorUI::PushID pushId(req.field);

	auto srcQuat = req.getFirstObjectValue<Quat4f>();
	auto value = srcQuat.euler();

	bool isMixedX = false;
	bool isMixedY = false;
	bool isMixedZ = false;

	if (req.objects.size() > 1) {
		for (auto& obj : req.objects) {
			const auto& tmp = req.field->getValue<Quat4f>(obj).euler();
			if (tmp.x != value.x) isMixedX = true;
			if (tmp.y != value.y) isMixedY = true;
			if (tmp.z != value.z) isMixedZ = true;
		}
	}

	struct Helper {
		static bool drawField(const char* name, float& newV, bool& isMixed) {
			auto mv = makeScopedValue(&EditorUI::showMixedValue, isMixed);
			auto tmpValue = Math::degrees(newV);
			if (EditorUI::DragFloat(name, &tmpValue)) {
				newV = Math::radians(tmpValue);
				return true;
			}
			return false;
		}
	};

	auto newV(value);

	bool updatedX = Helper::drawField("x", newV.x, isMixedX);
	bool updatedY = Helper::drawField("y", newV.y, isMixedY);
	bool updatedZ = Helper::drawField("z", newV.z, isMixedZ);
	if (!(updatedX || updatedY || updatedZ)) return;

	auto deltaValue = newV - value;
	for (auto& obj : req.objects) {
		auto v = req.field->getValue<Quat4f>(obj);
		auto v3 = v.euler();
		v.setEuler(v3 + deltaValue); // gimbal lock problem and floating point problem ..
		req.field->setValue<Quat4f>(obj, v);
	}
}

}