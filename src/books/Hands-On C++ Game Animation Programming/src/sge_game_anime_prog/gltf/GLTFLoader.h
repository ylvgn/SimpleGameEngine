#pragma once

#include "cgltf.h"
#include "GLTFInfo.h"

namespace sge {
/*
	The GLTFLoader presented here assumes that a file contains a single model only.
*/
class GLTFLoader : public NonCopyable {
public:
	using Info = GLTFInfo;

	~GLTFLoader();

	static void s_readFile(Info& outInfo, StrView filename);
	static void s_readMem(Info& outInfo, ByteSpan data, StrView filename);

private:
	void _readMem(Info& outInfo, ByteSpan data, StrView filename);

	// In this book, we will assume that a glTF file only contains one animated character.
	// It's safe to assume that the entire hierarchy of the glTF file can be treated as the skeleton of the model.
	void _loadAnimationClips();

	void _loadRestPose();
	void _loadBindPose();
	void _loadJointNames();
	void _loadSkeleton();
	void _loadMeshes();

	cgltf_data* _data	= nullptr;
	Info* _outInfo		= nullptr;
};

}

