#include "Skeleton.h"
#include <sge_game_anime_prog/gltf/GLTFInfo.h>

namespace sge {

void Skeleton::clear() {
	_restPose.clear();
	_bindPose.clear();
	_invBindPose.clear();
	_jointNames.clear();
}

void Skeleton::setRestPose(const Pose& p) {
	_restPose = p;
}

void Skeleton::setBindPose(const Pose& p) {
	_bindPose = p;
	_updateInverseBindPose();
}

void Skeleton::setJointNames(const Span<const String>& names) {
	_jointNames.clear();
	_jointNames.appendRange(names); // StringT need add copy constructor and operator=
}

void Skeleton::_updateInverseBindPose() {
	size_t jointCount = _bindPose.getJointCount();
	_invBindPose.resize(jointCount);

	for (int i = 0; i < jointCount; ++i) {
		Transform world = _bindPose.getGlobalTransform(i);
#if 0
		_invBindPose[i] = Transform::s_mat4(world).inverse();
#else
		// To generate an inverse bind pose matrix,
		// you don't have to convert the transform into a matrix and then invert it;
		// you could invert the transform and then convert it into a matrix.
		// The performance delta between the two is minimal.
		_invBindPose[i] = mat4f::s_transform(world.inverse());
#endif
	}
}

void Skeleton::create(const GLTFInfo& src) {
	setRestPose(src.restPose());
	setBindPose(src.bindPose());
	setJointNames(src.jointNames());
}

void Skeleton::getInvBindPose(Vector<mat4f>& out) const {
	out.clear();
	out.reserve(_invBindPose.size());
	out.appendRange(_invBindPose);
}

void Skeleton::getInvBindPose(Vector<dual_quat>& out) const {
	size_t jointCount = _bindPose.getJointCount();
	out.resize(jointCount);

	for (int i = 0; i < jointCount; ++i) {
		dual_quat4f world = _bindPose.getGlobalDualQuaternion(i);
		out[i] = world.conjugate();
	}
}

}