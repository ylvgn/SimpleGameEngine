#include "HingeSocketConstraintExample.h"

#include <sge_game_anime_prog/ik/CCDSolver.h>
#include <sge_game_anime_prog/ik/FABRIKSolver.h>

namespace sge {

template HingeSocketConstraintExample<CCDSolver>;
template HingeSocketConstraintExample<FABRIKSolver>;

template<class IKSolver>
HingeSocketConstraintExample<IKSolver>* HingeSocketConstraintExample<IKSolver>::instance() {
	static This s;
	return &s;
}

template<class IKSolver>
void HingeSocketConstraintExample<IKSolver>::onCreate() {
	Base::onCreate();
	static vec3f sLimitAxis = vec3f::s_up();

	size_t jointCount = _solver->getJointCount();
	for (int i = 0; i < jointCount; ++i) {
		_jointId2LimitAxis[i] = sLimitAxis;
	}
	_solver->setConstraintsHandler(&s_constraintHandler);
}

template<class IKSolver>
void HingeSocketConstraintExample<IKSolver>::s_constraintHandler(int i, IKSolver* solver) {
	if (solver == nullptr) return;

	auto* exampleThis = HingeSocketConstraintExample<IKSolver>::instance();
	auto& map = exampleThis->_jointId2LimitAxis;

	if (map.find(i) == map.end()) return;
	const vec3f& axis = map[i];

	const quat& thisLocalRot = solver->operator[](i).rotation;
	quat parentRot = i == 0
		? thisLocalRot
		: solver->getWorldTransform(i-1).rotation;

	vec3f currentHinge		= thisLocalRot * axis;
	vec3f desiredHinge		= parentRot * axis;
	quat4f currentToDesired = quat4f::s_fromTo(currentHinge, desiredHinge);

	solver->operator[](i).rotation = solver->operator[](i).rotation * currentToDesired;
}

}