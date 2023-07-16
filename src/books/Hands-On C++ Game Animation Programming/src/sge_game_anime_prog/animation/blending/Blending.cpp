#include "Blending.h"

namespace sge {

bool Blending::isInHierarchy(const Pose& pose, int parent, int target) {
	// if target is valid but parent is negative, will always return false
	// cuz the root parent's parent is always itself

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

void Blending::blend(Pose& out, const Pose& from, const Pose& to, float t, int blendRoot/*= -1*/) {
	size_t jointCount = out.getJointCount();
	for (int i = 0; i < jointCount; ++i) {

		// If two animations are blended using the whole hierarchy, means blendRoot < 0
		if (blendRoot >= 0 && !Blending::isInHierarchy(out, blendRoot, i)) {
			continue;
		}

		// blending must in local space
		out.setLocalTransform(i,
			from.getLocalTransform(i).lerp(to.getLocalTransform(i), t)
		);
	}
}

Pose Blending::makeAdditiveBasePose(const Skeleton& skeleton, const Clip& clip) {
	// It should sample the provided clip at the start time of the clip.
	// The result of that sample is the additive base pose
	Pose res(skeleton.restPose());
	clip.sample(res, clip.getStartTime());
	return res;
}

void Blending::add(Pose& out, const Pose& inputPose, const Pose& additivePose, const Pose& additiveBasePose, int blendRoot/*= -1*/) {
	// additiveBasePose comes from Blending::makeAdditiveBasePose

	// The additive blending formula is 
	// out pose = input pose + (additive pose – additive base pose)
	// using the local Transform of each joint
	size_t jointCount = additivePose.getJointCount();
	for (int i = 0; i < jointCount; ++i) {
		Transform input = inputPose.getLocalTransform(i);
		Transform additive = additivePose.getLocalTransform(i);
		Transform additiveBase = additiveBasePose.getLocalTransform(i);
		
		if (blendRoot >= 0 && !Blending::isInHierarchy(additivePose, blendRoot, i)) {
			continue;
		}

		Transform result;
		result.position = input.position + (additive.position - additiveBase.position);
		result.scale    = input.scale    + (additive.scale - additiveBase.scale);
/*
		Quaternions don't have a subtraction operator.
		To remove the rotation of quaternionA from quaternionB, multiply B by the inverse of A.
		The inverse of a quaternion applies the opposite of a rotation,
		which is why a quaternion multiplied by its inverse results in the identity.
*/
		result.rotation = input.rotation.normalize() * (additiveBase.rotation.inverse() * additive.rotation);
		out.setLocalTransform(i, result);
	}
}

}
