#include "AnimBaker.h"

namespace sge {

void AnimBaker::bakeAnimationClipToTex(SPtr<AnimTexture>& tex, const Skeleton* skel, const Clip& clip) {
	if (skel == nullptr) return;

	Pose	bindPose	= skel->bindPose();
	size_t	jointCount	= bindPose.getJointCount();
	size_t	texSize		= tex->texSize();

	float maxX = static_cast<float>(texSize - 1);
	for (int x = 0; x < texSize; ++x) {
		float t = static_cast<float>(x) / maxX; // 0~1
		float time = (clip.getStartTime() + clip.getDuration()) * t;
		clip.sample(bindPose, time);

		// write current pose data(all bones) into texture
		for (int i = 0, y = 0; i < jointCount; ++i, y += 3) {
			Transform world = bindPose.getGlobalTransform(i);
			tex->setTexel(x, y + 0, world.position);
			tex->setTexel(x, y + 1, world.rotation);
			tex->setTexel(x, y + 2, world.scale);
		}
	}

	// Before the Bake function returns,
	// call the `uploadToGpu` function on the provided animation texture.
	// This will make the texture usable immediately after it has been baked
	tex->uploadToGpu();
}

}