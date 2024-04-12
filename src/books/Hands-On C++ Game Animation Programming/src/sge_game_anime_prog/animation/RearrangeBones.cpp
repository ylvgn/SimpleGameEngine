#include "RearrangeBones.h"

namespace sge {

void RearrangeBones::s_rearrange(SPtr<Skeleton>& outSkeleton,
								 Span<Mesh> outMeshes,
								 Span<FastClip> outClips)
{
	_s_internal_rearrange(outSkeleton, outMeshes, outClips);
}

template<typename T>
void RearrangeBones::_rearrangeClip(ClipT<T>& out) {
	for (int i = 0; i < out.getTrackCount(); ++i) {
		auto* p = out.getTrack_noCheck(i);
		SGE_ASSERT(p != nullptr);
		int oldJointId = p->jointId;
		int newJointId = _rearrangeIdx(oldJointId);
		p->jointId = newJointId;
	}
}

template<typename T>
void RearrangeBones::_s_internal_rearrange(SPtr<Skeleton>& outSkeleton, Span<Mesh> outMeshes, Span< ClipT<T> > outClips) {
	RearrangeBones re;

	re._create(outSkeleton->bindPose());
	re._rearrangeSkeleton(outSkeleton);

	for (auto& outMesh : outMeshes) re._rearrangeMesh(outMesh);
	for (auto& outClip : outClips) re._rearrangeClip(outClip);
}

int RearrangeBones::_rearrangeIdx(int oldJointIndex) const {
	SGE_ASSERT(_o2n.find(oldJointIndex) != _o2n.end());
	return _o2n.at(oldJointIndex);
}

int RearrangeBones::_originalIdx(int newJointIndex) const {
	SGE_ASSERT(_n2o.find(newJointIndex) != _n2o.end());
	return _n2o.at(newJointIndex);
}

void RearrangeBones::_clear() {
	_o2n.clear();
	_n2o.clear();
}

void RearrangeBones::_create(const Pose& bindPose) {
	_clear();

	size_t jointCount = bindPose.getJointCount();
	if (jointCount == 0) {
		return;
	}

	Vector< Vector<int> > children;
	children.resize(jointCount);

	Vector<int> processIds;
	processIds.resize(jointCount);

	int hh = 0, tt = 0;
	for (int i = 0; i < jointCount; ++i) {
		int parent = bindPose.getParent(i);
		if (parent >= 0) {
			children[parent].push_back(i);
		} else {
			processIds[tt++] = i;
		}
	}

	int rearrangeId = 0;
	while (hh < tt) {
		int originalId = processIds[hh++];
		const auto& childs = children[originalId];
		for (const auto& child : childs) {
			processIds[tt++] = child;
		}

		_o2n[originalId]  = rearrangeId;
		_n2o[rearrangeId] = originalId;
		rearrangeId++;
	}

	_n2o[-1] = -1;
	_o2n[-1] = -1;
}

void RearrangeBones::_rearrangeSkeleton(SPtr<Skeleton>& out) {
	size_t jointCount = out->getJointCount();
	if (jointCount == 0) {
		return;
	}

	const auto& restPose   = out->restPose();
	const auto& bindPose   = out->bindPose();
	const auto& jointNames = out->jointNames();

	Pose newRestPose (jointCount);
	Pose newBindPose (jointCount);
	Vector<String> newJointNames;
	newJointNames.resize(jointCount);

	for (int i = 0; i < jointCount; ++i) {
		int oldJointId	= _originalIdx(i);
		int oldParent	= bindPose.getParent(oldJointId);
		int newParent	= _rearrangeIdx(oldParent);

		newJointNames[i].assign(jointNames[oldJointId]);

		newRestPose.setLocalTransform(i, restPose.getLocalTransform(oldJointId));
		newBindPose.setLocalTransform(i, bindPose.getLocalTransform(oldJointId));

		newRestPose.setParent(i, newParent);
		newBindPose.setParent(i, newParent);
	}

	out->setRestPose(newRestPose);
	out->setBindPose(newBindPose);
	out->setJointNames(newJointNames);
}

void RearrangeBones::_rearrangeMesh(Mesh& out) {
/*
	Changing the mesh in this way only edits the CPU copy of the mesh.
	Call 'uploadToGpu' to upload the new attribute to the GPU as well
*/
	auto& j = out.jointInfluences;
	for (int i = 0; i < j.size(); ++i) {
		// The weights of the joint don't need to be edited since the joint itself didn't change;
		// only its index in the array changed, only the value of x,y,z,w.
		// influences[i] is still related to weights[i]
		j[i].x = _rearrangeIdx(j[i].x);
		j[i].y = _rearrangeIdx(j[i].y);
		j[i].z = _rearrangeIdx(j[i].z);
		j[i].w = _rearrangeIdx(j[i].w);
	}
	out.uploadToGpu();
}

// explict template instantiation
template void RearrangeBones::_s_internal_rearrange(SPtr<Skeleton>& outSkeleton, Span<Mesh> outMeshes, Span<Clip> outClips);
template void RearrangeBones::_s_internal_rearrange(SPtr<Skeleton>& outSkeleton, Span<Mesh> outMeshes, Span<FastClip> outClips);

}