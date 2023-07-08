#include "Blending.h"

namespace sge {

bool Blending::isInHierarchy(const Pose& pose, int parent, int target) {
	// if parent is negative, will always return false

	if (target == parent) {
		return true;
	}

	int p = pose.getParent(target);
	while (p >= 0) {
		if (p == parent) return true;
		p = pose.getParent(p);
	}

	return false;
}

void Blending::blend(Pose& out, const Pose& from, const Pose& to, float t, int blendRoot) {
	size_t jointCount = out.getJointCount();
	for (int i = 0; i < jointCount; ++i) {

		// If two animations are blended using the whole hierarchy, means blendRoot < 0
		if (blendRoot >= 0) {
			if (!Blending::isInHierarchy(out, blendRoot, i)) {
				continue;
			}
		}

		// blending must in local space
		out.setLocalTransform(i,
			from.getLocalTransform(i).lerp(to.getLocalTransform(i), t)
		);
	}
}

}
