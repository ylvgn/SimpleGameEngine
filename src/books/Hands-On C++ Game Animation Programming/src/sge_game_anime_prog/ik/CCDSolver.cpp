#include "CCDSolver.h"

namespace sge {

Transform CCDSolver::getGlobalTransform(int i) const {
	Transform world(_ikChains[i]);
	for (int j = i - 1; j >= 0; --j) {
		world = Transform::s_combine(_ikChains[j], world);
	}
	return world;
}

bool CCDSolver::solve(const Transform& target) {
/*
	Cyclic Coordinate Descent algorithm:
		First, there is the goal, the point in space you are trying to touch.
		Next is the IK chain, which is a list of all of the joints that will need to rotate to reach the goal.
		Finally, there is the end-effector, which is the last joint in the chain (the one that needs to touch the goal).

	A transform is provided, but only the position component of the transform is ever used.
*/

	size_t jointCount = _ikChains.size();
	if (jointCount < 2) return false;

	float thresholdSq = _threshold * _threshold;
	const auto& goal = target.position;

	constexpr const float kEpsilon = 0.00001f;
	int last = static_cast<int>(jointCount) - 1;
	for (int i = 0; i < _stepCount; ++i) {
		vec3 effector = getGlobalTransform(last).position;
		if ((goal - effector).lenSq() < thresholdSq) {
			return true;
		}

		for (int j = last - 1; j >= 0; --j) {
			effector		= getGlobalTransform(last).position;
			Transform world = getGlobalTransform(j);

			const vec3& worldPos	= world.position;
			const quat& worldRot	= world.rotation;

			vec3 toEffector			= effector - worldPos;
			vec3 toGoal				= goal - worldPos;

#if 1 // calc in world space
			quat effectorToGoal = quat::s_identity();
			if (toGoal.lenSq() > kEpsilon) { // maybe no need ???
				// There is an edge case in which the vector pointing to the effector or to the goal could be a zero vector
				effectorToGoal = quat::s_fromTo(toEffector, toGoal);
			}

			quat worldRotated	= worldRot * effectorToGoal;		// current rotation (world) rotate more to effectorToGoal 
			quat localRotated	= worldRotated * worldRot.inverse();// we want in local space, cuz _ikChains saved Transform in local space.
#else // calc in local space
			toEffector			= worldRot.inverse() * toEffector;
			toGoal				= worldRot.inverse() * toGoal;
			quat localRotated	= quat::s_fromTo(toEffector, toGoal);
#endif
			_ikChains[j].rotation = localRotated * _ikChains[j].rotation;	// apply localRotate in current rotation (local)

			effector = getGlobalTransform(last).position;
			if ((goal - effector).lenSq() < thresholdSq) {
				return true;
			}
		}
	}
	return false;
}

}
