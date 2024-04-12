#pragma once

#include <sge_game_anime_prog/rendering/Mesh.h>
#include "Clip.h"

namespace sge {
/*
	take advantage of the optimized vertion of Pose's getMatrixPalette
	Rearranging the skeleton is not enough.
	Since the joint indices moved, any clips or meshes that reference this skeleton are now broken.
*/

class RearrangeBones : public NonCopyable {
public:
	static void s_rearrange(SPtr<Skeleton>& outSkeleton, Span<Mesh> outMeshes, Span<FastClip> outClips);
	static void s_rearrange(SPtr<Skeleton>& outSkeleton, Span<Mesh> outMeshes, Span<Clip> outClips);
private:
	void _create(const Pose& bindPose);
	void _clear();

	void _rearrangeSkeleton(SPtr<Skeleton>& out);
	void _rearrangeMesh(Mesh& out);

	template<class T>
	void _rearrangeClip(ClipT<T>& out);

	template<class T>
	static void _s_internal_rearrange(SPtr<Skeleton>& outSkeleton, Span<Mesh> outMeshes, Span< ClipT<T> > outClips);

	int _rearrangeIdx(int oldJointIndex) const;
	int _originalIdx(int newJointIndex)  const;

	Map<int, int>	_n2o; // new to old (index) forward
	Map<int, int>	_o2n; // old to new (index) backward
};

}