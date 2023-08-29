#include "RearrangeBones.h"

namespace sge {

#if MYTEST
// explict template instantiation
template void RearrangeBones::s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span< Clip > outClips);
template void RearrangeBones::s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span< FastClip > outClips);

template <typename T>
void RearrangeBones::s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span< TClip<T> > outClips) {
	RearrangeBones re;
	re.create(outSkeleton);

	re.rearrangeSkeleton(outSkeleton);
	for (auto& mesh : outMeshes) re.rearrangeMesh(mesh);
	for (auto& clip : outClips)  re.rearrangeClip(clip);
}
#else
void RearrangeBones::s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<Clip> outClips) {
	RearrangeBones re;
	re.create(outSkeleton);

	re.rearrangeSkeleton(outSkeleton);
	for (auto& mesh : outMeshes) re.rearrangeMesh(mesh);
	for (auto& clip : outClips)  re.rearrangeClip(clip);
}

void RearrangeBones::s_rearrange(Skeleton& outSkeleton, Span<Mesh> outMeshes, Span<FastClip> outClips) {
	RearrangeBones re;
	re.create(outSkeleton);

	re.rearrangeSkeleton(outSkeleton);
	for (auto& mesh : outMeshes) re.rearrangeMesh(mesh);
	for (auto& clip : outClips)  re.rearrangeClip(clip);
}
#endif


int RearrangeBones::_rearrangeIdx(int oldJointIndex) const {
	SGE_ASSERT(_o2n.find(oldJointIndex) != _o2n.end());
	return _o2n.at(oldJointIndex);
}

int RearrangeBones::_originalIdx(int newJointIndex) const {
	SGE_ASSERT(_n2o.find(newJointIndex) != _n2o.end());
	return _n2o.at(newJointIndex);
}

void RearrangeBones::clear() {
	_o2n.clear();
	_n2o.clear();
	_skeleton.clear();
}

void RearrangeBones::create(const Skeleton& skeleton) {
	clear();
	const auto& bindPose = skeleton.bindPose();
	const auto& restPose = skeleton.restPose();

	size_t jointCount = restPose.getJointCount();
	if (jointCount == 0) {
		return;
	}

	_skeleton.setBindPose(bindPose);
	_skeleton.setRestPose(restPose);
	_skeleton.setJointNames(skeleton.jointNames());

	Vector< Vector<int> > children;
	children.resize(jointCount);

	Vector<int> processIds;
	processIds.resize(jointCount);

	int hh = 0;
	int tt = 0;
	for (int i = 0; i < jointCount; ++i) {
		int parent = restPose.getParent(i);
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

void RearrangeBones::rearrangeSkeleton(Skeleton& out) {
	size_t jointCount = _skeleton.getJointCount();
	if (jointCount == 0) {
		return;
	}

	const auto& restPose   = _skeleton.restPose();
	const auto& bindPose   = _skeleton.bindPose();
	const auto& jointNames = _skeleton.jointNames();

	Pose newRestPose (jointCount);
	Pose newBindPose (jointCount);
	Vector<String> newJointNames;
	newJointNames.resize(jointCount);

	for (int i = 0; i < jointCount; ++i) {
		int oldJointId = _originalIdx(i);
		int oldParent  = bindPose.getParent(oldJointId);
		int newParent  = _rearrangeIdx(oldParent);

		newJointNames[i] = jointNames[oldJointId];

		newRestPose.setLocalTransform(i, restPose.getLocalTransform(oldJointId));
		newBindPose.setLocalTransform(i, bindPose.getLocalTransform(oldJointId));

		newRestPose.setParent(i, newParent);
		newBindPose.setParent(i, newParent);
	}

	out.setRestPose(newRestPose);
	out.setBindPose(newBindPose);
	out.setJointNames(newJointNames);
}

void RearrangeBones::rearrangeMesh(Mesh& out) {
/*
	Changing the mesh in this way only edits the CPU copy of the mesh.
	Call UpdateOpenGLBuffers to upload the new attribute to the GPU as well
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

#if MYTEST
template<typename T>
void RearrangeBones::rearrangeClip(TClip<T>& out) {
	for (int i = 0; i < out.size(); ++i) {
		int oldJointId = out.getJointIdAtIndex(i);
		int newJointId = _rearrangeIdx(oldJointId);
		out.setJointIdAtIndex(i, newJointId);
	}
}
#else
void RearrangeBones::rearrangeClip(Clip& out) {
	for (int i = 0; i < out.getTrackCount(); ++i) {
		int oldJointId = out.getJointIdAtIndex(i);
		int newJointId = _rearrangeIdx(oldJointId);
		out.setJointIdAtIndex(i, newJointId);
	}
}

void RearrangeBones::rearrangeClip(FastClip& out) {
	for (int i = 0; i < out.getTrackCount(); ++i) {
		int oldJointId = out.getJointIdAtIndex(i);
		int newJointId = _rearrangeIdx(oldJointId);
		out.setJointIdAtIndex(i, newJointId);
	}
}
#endif
}