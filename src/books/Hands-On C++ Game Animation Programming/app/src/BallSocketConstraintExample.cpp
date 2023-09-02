#include "BallSocketConstraintExample.h"

#include <sge_game_anime_prog/ik/CCDSolver.h>
#include <sge_game_anime_prog/ik/FABRIKSolver.h>

namespace sge {

template BallSocketConstraintExample<CCDSolver>;
template BallSocketConstraintExample<FABRIKSolver>;

template<class IKSolver>
BallSocketConstraintExample<IKSolver>* BallSocketConstraintExample<IKSolver>::instance() {
	static This s;
	return &s;
}

template<class IKSolver>
void BallSocketConstraintExample<IKSolver>::onCreate() {
	Base::onCreate();
	constexpr float kLimitDegrees = 60.f;

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
	const quat& thisLocalRot = solver->operator[](i).rotation;

	quat parentRot = i == 0
		? thisLocalRot
		: solver->getWorldTransform(i-1).rotation;

	quat thisRot = solver->getWorldTransform(i).rotation;

	vec3f parentDir = parentRot * vec3f::s_forward();
	vec3f thisDir	= thisRot   * vec3f::s_forward();

	float angle = parentDir.angle(thisDir);

	if (angle > limitAngle) {
#if 1 // calc in world space
		vec3f correction	= parentDir.cross(thisDir);
		quat worldRotation	= parentRot * quat::s_angleAxis(limitAngle, correction);
		quat localRotation	= worldRotation * parentRot.inverse();
#else // calc in local space
		vec3f correction	= parentRot.inverse() * parentDir.cross(thisDir);
		quat localRotation	= thisLocalRot * quat::s_angleAxis(limitAngle, correction);
#endif
		solver->operator[](i).rotation = localRotation;
	}
}

}

