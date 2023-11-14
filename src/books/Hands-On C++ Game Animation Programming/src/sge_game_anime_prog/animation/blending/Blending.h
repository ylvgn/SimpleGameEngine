#pragma once

#include "../Pose.h"
#include "../Clip.h"
#include "../Skeleton.h"

namespace sge {
/*
	* Animation Blending is simply seen as lerp from PoseA to PoseB using Transfrom interpolate.

	* Additive animations are most often used to create new animation variants
		* for example, mixing a walking animation with a crouching pose to create a crouched walk.
*/
struct Blending {
	Blending() = delete;

	static bool isInHierarchy(const Pose& pose, int parent, int target);
	static void blend(Pose& out, const Pose& from, const Pose& to, float t, int blendRoot = -1);

	static Pose makeAdditiveBasePose(const Skeleton& skeleton, const Clip& clip);
	static void add(Pose& out, const Pose& inputPose, const Pose& additivePose, const Pose& additieBasePose, int blendRoot = -1);
};

}