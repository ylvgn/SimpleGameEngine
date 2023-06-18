#pragma once

#include "cgltf.h"
#include <sge_game_anime_prog/animation/Clip.h>

namespace sge {

struct GLTFLoader_Info {
	Pose restPose;
	Vector<String> jointNames;
	Vector<Clip> animationClips;

	void clear() {
		restPose.resize(0);
		jointNames.clear();
		animationClips.clear();
	}
};

class GLTFLoader : public NonCopyable {
public:
	using Info = GLTFLoader_Info;

	~GLTFLoader();

	static void s_readFile(Info& outInfo, StrView filename);
	static void s_readMem(Info& outInfo, ByteSpan data, StrView filename);

private:
	void _readMem(Info& outInfo, ByteSpan data, StrView filename);

	// In this book, we will assume that a glTF file only contains one animated character.
	// It's safe to assume that the entire hierarchy of the glTF file can be treated as the skeleton of the model.
	void _loadRestPose();
	void _loadJointNames();
	void _loadAnimationClips();

	cgltf_data* _data			= nullptr;
	GLTFLoader_Info* _outInfo	= nullptr;
};

}

