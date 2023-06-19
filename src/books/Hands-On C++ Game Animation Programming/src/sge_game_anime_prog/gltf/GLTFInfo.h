#pragma once

#include <sge_game_anime_prog/animation/Clip.h>
#include <sge_game_anime_prog/animation/Skeleton.h>

namespace sge {

class GLTFInfo : public NonCopyable {
public:
	Skeleton skeleton;
	Vector<Clip> animationClips;

	inline const Pose& restPose() const { return skeleton.restPose(); }
	inline const Pose& bindPose() const { return skeleton.bindPose(); }
	inline Span<const String> jointNames() const { return skeleton.jointNames(); }

	void clear();
};
	
}
