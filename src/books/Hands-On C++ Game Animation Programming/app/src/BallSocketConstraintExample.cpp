#include "BallSocketConstraintExample.h"

#include <sge_game_anime_prog/ik/CCDSolver.h>
#include <sge_game_anime_prog/ik/FABRIKSolver.h>

namespace sge {

template<class IKSolver>
BallSocketConstraintExample<IKSolver>* BallSocketConstraintExample<IKSolver>::instance() {
	static This s;
	return &s;
}

template<class IKSolver>
void BallSocketConstraintExample<IKSolver>::onCreate(Request& req) {
	Base::onCreate(req);
	const constexpr float kLimitDegrees = 60.f;

	size_t jointCount =_solver->getJointCount();
	for (int i = 0; i < jointCount; ++i) {
		_jointId2LimitAngle[i] = Math::radians(kLimitDegrees);
	}
	_solver->setConstraintsHandler(&s_constraintHandler);
}

template<class IKSolver>
void BallSocketConstraintExample<IKSolver>::s_constraintHandler(int i, IKSolver* solver) {
	if (solver == nullptr) return;

	auto* exampleThis = BallSocketConstraintExample<IKSolver>::instance();
	auto& map = exampleThis->_jointId2LimitAngle;

	if (map.find(i) == map.end()) return;
	float limitAngle = map[i];

	auto& localTrans = solver->getLocalTransform(i);
	const quat4f& thisLocalRot = localTrans.rotation;
	const quat4f& parentRot = i == 0
		? thisLocalRot
		: solver->getWorldTransform(i-1).rotation;

	const quat4f& thisRot = solver->getWorldTransform(i).rotation;

	vec3f thisDir	= thisRot   * vec3f::s_forward();
	vec3f parentDir = parentRot * vec3f::s_forward();

	float angle = parentDir.angle(thisDir);
	if (angle > limitAngle) {
		vec3f  worldRotateAxis			 = parentDir.cross(thisDir);
		quat4f parentDirToLimitedThisDir = quat4f::s_angleAxis(limitAngle, worldRotateAxis);
		quat4f worldLimitedRotation		 = parentRot * parentDirToLimitedThisDir;
		quat4f localLimitedRotation		 = worldLimitedRotation * parentRot.inverse();
		localTrans.rotation				 = localLimitedRotation;
	}
}

template BallSocketConstraintExample<CCDSolver>;
template BallSocketConstraintExample<FABRIKSolver>;

}

