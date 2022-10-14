#pragma once

#include "Component.h"

namespace sge {

class CTransform : public Component {
	SGE_OBJECT_TYPE(CTransform, Component)
public:
	Vec3f	localPosition{0,0,0};
	Vec3f	localScale{0,0,0};
	Quat4f	localRotation = Quat4f::s_identity();

	void onFormat(fmt::format_context& ctx) const;
};

SGE_FORMATTER(CTransform)

} // namespace