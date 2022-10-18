#include "CTransform.h"

namespace sge {

const TypeInfo* CTransform::s_getType() {
	using This = CTransform;
	class Ti : public CTransform::TI_Base {
	public:
		Ti() {
			static FieldInfo fi[] = {
				FieldInfo("position", &This::position),
				FieldInfo("rotate",	&This::rotate),
				FieldInfo("scale", &This::scale),
			};
			setFields(fi);
		}
	};
	static Ti ti;
	return &ti;
}

void CTransform::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "position=({},{}, {}), rotate=({}, {}, {}, {}), scale=({}, {}, {})",
		position.x, position.y, position.z,
		rotate.x, rotate.y, rotate.z, rotate.w,
		scale.x, scale.y, scale.z
	);
}

} // namespace