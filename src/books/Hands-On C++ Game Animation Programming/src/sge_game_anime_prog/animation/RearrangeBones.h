#pragma once

#include "Skeleton.h"
#include <sge_game_anime_prog/rendering/Mesh.h>
#include "Clip.h"

namespace sge {

/*
	take advantage of the optimized vertion of Pose's getMatrixPalette
	Rearranging the skeleton is not enough. Since the joint indices moved,
	any clips or meshes that reference this skeleton are now broken.
*/

class RearrangeBones : public NonCopyable {
private:
	template<typename T> static void s_internalRearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span< ClipT<T> > outClips);
	template<typename T> void _internalRearrangeClip(ClipT<T>& out);
public:

	static void s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<Clip> outClips)		{ s_internalRearrange(outSkeleton, outMeshes, outClips); }
	static void s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<FastClip> outClips)	{ s_internalRearrange(outSkeleton, outMeshes, outClips); }

	void rearrangeClip(Clip& out)		{ _internalRearrangeClip(out); }
	void rearrangeClip(FastClip& out)	{ _internalRearrangeClip(out); }

	void create(const Skeleton& skeleton);
	void clear();

	void rearrangeSkeleton(Skeleton& out);
	void rearrangeMesh(Mesh& out);

private:
	int _rearrangeIdx(int oldJointIndex) const;
	int _originalIdx(int newJointIndex) const;

	Map<int, int> _n2o; // new to old (index) forward
	Map<int, int> _o2n; // old to new (index) backward

	Skeleton _skeleton;
};

}