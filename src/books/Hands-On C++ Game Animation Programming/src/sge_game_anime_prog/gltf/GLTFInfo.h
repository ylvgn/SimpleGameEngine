#pragma once

#include <sge_game_anime_prog/animation/Clip.h>
#include <sge_game_anime_prog/animation/Skeleton.h>
#include <sge_game_anime_prog/rendering/Mesh.h>

namespace sge {

struct GLTFInfo {
public:
	void clear();

	inline const Pose& restPose()			const { return skeleton.restPose(); }
	inline const Pose& bindPose()			const { return skeleton.bindPose(); }
	inline Span<const String> jointNames()	const { return skeleton.jointNames(); }
	inline Span<const String> clipNames()	const { return animationClipNames; }

	Skeleton		skeleton;
	Vector<Clip>	animationClips;
	Vector<String>	animationClipNames;

	Vector<Mesh>	skinMeshes;
	Vector<Mesh>	staticMeshes;
};
	
}
