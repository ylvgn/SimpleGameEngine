#pragma once
#include "Component.h"

namespace sge {

class Transform : public Component {
	SGE_TYPEOF_DEFINE(Transform, Component)
public:
	Transform(GameObject* owner) : Base(owner) {};

	Vec3f	localPosition{0,0,0};
	Vec3f	localScale{0,0,0};
	Quat4f	localRotation = Quat4f::s_identity();

	void onFormat(fmt::format_context& ctx) const;

};

SGE_FORMATTER(Transform)
} // namespace