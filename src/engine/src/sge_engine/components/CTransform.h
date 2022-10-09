#pragma once

#include "Component.h"

namespace sge {

class CTransform;
template<> const TypeInfo* TypeInfo_get<CTransform>();

class CTransform : public Component {
	SGE_TYPEOF_DEFINE(CTransform, Component)
public:
	CTransform(Entity* entity) : Base(entity) {};

	Vec3f	localPosition{0,0,0};
	Vec3f	localScale{0,0,0};
	Quat4f	localRotation = Quat4f::s_identity();

	void onFormat(fmt::format_context& ctx) const;
};

SGE_FORMATTER(CTransform)

} // namespace