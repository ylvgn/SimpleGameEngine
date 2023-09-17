#include "IKLeg.h"

namespace sge {

IKLeg::IKLeg()
	: _hipIndex(0)
	, _kneeIndex(0)
	, _ankleIndex(0)
	, _toeIndex(0)
	, _ankleToGroundOffset(0)
{
	_solver.resize(3);
}

// for Woman.gltf
	// hipJointName,	kneeJointName,	ankleJointName, toeJointName
	// "LeftUpLeg",		"LeftLeg",		"LeftFoot",		"LeftToeBase"
void IKLeg::setByJointNames(const Skeleton& skeleton, StrView hipJointName, StrView kneeJointName, StrView ankleJointName, StrView toeJointName) {
	size_t jointCount = skeleton.getJointCount();
	for (int i = 0; i < jointCount; ++i) {
		StrView jointName = skeleton.getJointName(i);
		if (jointName == hipJointName) {
			_hipIndex = i;
		}
		else if (jointName == kneeJointName) {
			_kneeIndex = i;
		}
		else if (jointName == ankleJointName) {
			_ankleIndex = i;
		}
		else if (jointName == toeJointName) {
			_toeIndex = i;
		}
	}
}

void IKLeg::solveForLeg(const Transform& model, const Pose& animatedPose, const vec3f& ankleTargetPosition) {
	_solver.setLocalTransform(0, Transform::s_combine(model, animatedPose.getGlobalTransform(_hipIndex)));
	_solver.setLocalTransform(1, animatedPose.getLocalTransform(_kneeIndex));
	_solver.setLocalTransform(2, animatedPose.getLocalTransform(_ankleIndex));

	_outPose = animatedPose;

	vec3f targetPos = ankleTargetPosition + vec3f::s_up() * _ankleToGroundOffset;
	Transform target(targetPos, quat::s_identity(), vec3f::s_one());
	_solver.solve(target);

	int hipRoot = animatedPose.getParent(_hipIndex);
	Transform rootWorld = Transform::s_combine(model, animatedPose.getGlobalTransform(hipRoot));
	_outPose.setLocalTransform(_hipIndex,   Transform::s_combine(rootWorld.inverse(), _solver.getLocalTransform(0)));
	_outPose.setLocalTransform(_kneeIndex,  _solver.getLocalTransform(1));
	_outPose.setLocalTransform(_ankleIndex, _solver.getLocalTransform(2));
}

}