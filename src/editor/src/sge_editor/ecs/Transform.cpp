#include "Transform.h"

namespace sge {

const TypeInfo* Transform::s_getType() {
	class Ti : public TI_Base {
	public:
		Ti() {
			static FieldInfo fi[] = {
				FieldInfo("localPosition",	&This::localPosition),
				FieldInfo("localScale",		&This::localScale),
				FieldInfo("localRotation",	&This::localRotation),
			};
			setFieldInfo(fi);
		}
	};
	static Ti ti;
	return &ti;
}

void Transform::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "localPosition=({},{}, {}), localScale=({}, {}, {}), localRotation=({}, {}, {}, {})",
		localPosition.x, localPosition.y, localPosition.z,
		localScale.x, localScale.y, localScale.z,
		localRotation.x, localRotation.y, localRotation.z, localRotation.w);
}

} // namespace