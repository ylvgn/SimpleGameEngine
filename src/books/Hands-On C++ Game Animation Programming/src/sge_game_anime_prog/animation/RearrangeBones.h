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
public:
	static void s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<Clip> outClips) {
		_s_internal_rearrange(outSkeleton, outMeshes, outClips);
	}
	static void s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<FastClip> outClips) {
		_s_internal_rearrange(outSkeleton, outMeshes, outClips);
	}
	static void s_rearrange(SPtr<Skeleton>& outSkeleton, Span<Mesh> outMeshes, Span<FastClip> outClips) {
		_s_internal_rearrange(*outSkeleton.ptr(), outMeshes, outClips);
	}
private:
	void _create(Skeleton& outSkeleton);
	void _clear();

	void _rearrangeMesh(Mesh& out);

	void _rearrangeClip(Clip& out)		{ _internalRearrangeClip(out); }
	void _rearrangeClip(FastClip& out)	{ _internalRearrangeClip(out); }

	void _rearrangeSkeleton();

	int _rearrangeIdx(int oldJointIndex) const;
	int _originalIdx(int newJointIndex) const;

	template<typename T> static void _s_internal_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span< ClipT<T> > outClips);
	template<typename T> void _internalRearrangeClip(ClipT<T>& out);

	Map<int, int> _n2o; // new to old (index) forward
	Map<int, int> _o2n; // old to new (index) backward

	Skeleton* _outSkeleton = nullptr;
};

}