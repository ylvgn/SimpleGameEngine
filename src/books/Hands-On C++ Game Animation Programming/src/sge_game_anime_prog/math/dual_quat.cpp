#include "dual_quat.h"

#include "Transform.h"

namespace sge {

dual_quat dual_quat::s_Transform(const Transform& t) {
	// The resulting dual quaternion does not need to be normalized.
	return dual_quat(t.rotation, t.position);
}

}
