#pragma once

#include "cgltf.h"
#include "GLTFInfo.h"

#define M4_3X3MINOR2(c0, c1, c2, r0, r1, r2) \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) - \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) + \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

namespace sge {

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

	cgltf_data* _data	= nullptr;
	Info* _outInfo		= nullptr;
};

}

