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

#define MYTEST 0

class RearrangeBones : public NonCopyable {
public:

#if MYTEST
	template<typename T>
	static void s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span< TClip<T> > outClips);
#else
	//what if spanCast<Clip, FastClip>(_fastClips); pass into line 25, is it danger ???
	static void s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<Clip> outClips);
	static void s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<FastClip> outClips);
#endif

	void create(const Skeleton& skeleton);
	void clear();

	void rearrangeSkeleton(Skeleton& out);
	void rearrangeMesh(Mesh& out);
#if MYTEST
	template<typename T> void rearrangeClip(TClip<T>& out);
#else
	void rearrangeClip(Clip& out);
	void rearrangeClip(FastClip& out);
#endif

private:
	int _rearrangeIdx(int oldJointIndex) const;
	int _originalIdx(int newJointIndex) const;

	Map<int, int> _n2o; // new to old (index) forward
	Map<int, int> _o2n; // old to new (index) backward

	Skeleton _skeleton;
};

}