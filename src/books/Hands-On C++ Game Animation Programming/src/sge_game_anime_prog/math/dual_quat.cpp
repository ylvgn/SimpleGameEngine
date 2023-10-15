#include "dual_quat.h"
#include "Transform.h"

namespace sge {

void dual_quat::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "dual_qual({}, {}, {}, {}, {}, {}, {}, {})", x, y, z, w, dx, dy, dz, dw);
}

dual_quat dual_quat::s_Transform(const Transform& t) {
	// The resulting dual quaternion does not need to be normalized.
	return dual_quat(t.rotation, t.position);
}

}
