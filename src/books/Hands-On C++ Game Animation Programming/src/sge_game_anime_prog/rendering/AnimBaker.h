#pragma once

#include "AnimTexture.h"

#include <sge_game_anime_prog/animation/Skeleton.h>
#include <sge_game_anime_prog/animation/Clip.h>

namespace sge {
/*
	The skeleton is important as it provides the rest pose,
	which will be used for any joint that isn't present in the animation clip.
	Every joint of the skeleton will get baked into the texture.
*/

struct AnimBaker {
	AnimBaker() = delete;

	// This Bake function will sample the animation at set intervals
	// and write the skeleton hierarchy for each sample into a floating-point texture.
	static void bakeAnimationClipToTex(SPtr<AnimTexture>& tex, const Skeleton* skel, const Clip& clip);
};

}