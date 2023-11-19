#pragma once

#include "AnimTexture.h"

namespace sge {
/*
	The skeleton is important as it provides the rest pose,
	which will be used for any joint that isn't present in the animation clip.
	Every joint of the skeleton will get baked into the texture.
*/
	
// forward declare with template class
class Skeleton;
template<typename TRACK> struct ClipT;
template<typename VTRACK, typename QTRACK> struct TransformTrackT;
template<typename T, size_t N> struct Track;

struct AnimBaker {
	AnimBaker() = delete;

	using VectorTrack		= Track<vec3f, 3>;
	using QuaternionTrack	= Track<quat4f, 4>;
	using TransformTrack	= TransformTrackT<VectorTrack, QuaternionTrack>;
	using Clip				= ClipT<TransformTrack>;

	// This Bake function will sample the animation at set intervals
	// and write the skeleton hierarchy for each sample into a floating-point texture.
	static void bakeAnimationClipToTex(SPtr<AnimTexture>& tex, const Skeleton* skel, const Clip& clip);
};

}