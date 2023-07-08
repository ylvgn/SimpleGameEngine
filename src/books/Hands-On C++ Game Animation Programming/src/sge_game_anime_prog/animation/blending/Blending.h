#pragma once

#include "../Pose.h"
#include "../Clip.h"
#include "../Skeleton.h"

namespace sge {

struct Blending {
	Blending() = delete;

	static bool isInHierarchy(const Pose& pose, int parent, int target);
	static void blend(Pose& out, const Pose& from, const Pose& to, float t, int blendRoot);
};

}