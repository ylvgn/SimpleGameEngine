#include "CCDSolver.h"

namespace sge {

Transform CCDSolver::getGlobalTransform(int i) const {
	SGE_ASSERT(i >= 0 || i < _ikChains.size());
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
		vec3f effector = getGlobalTransform(last).position;
		if ((goal - effector).lenSq() < thresholdSq) {
			return true;
		}

		for (int j = last - 1; j >= 0; --j) {
			effector		= getGlobalTransform(last).position;
			Transform world = getGlobalTransform(j);

			const vec3f&  worldPos	= world.position;
			const quat4f& worldRot	= world.rotation;

			vec3f toEffector		= effector - worldPos;
			vec3f toGoal			= goal - worldPos;

#if 1 // calc in world space
			quat4f effectorToGoal = quat4f::s_identity();
			if (toGoal.lenSq() > kEpsilon) { // maybe no need ???
				// There is an edge case in which the vector pointing to the effector or to the goal could be a zero vector
				effectorToGoal = quat4f::s_fromTo(toEffector, toGoal);
			}

			quat4f worldRotated	= worldRot * effectorToGoal;		// current rotation (world) rotate more to effectorToGoal 
			quat4f localRotated	= worldRotated * worldRot.inverse();// we want in local space, cuz _ikChains saved Transform in local space.
#else // calc in local space
			toEffector			= worldRot.inverse() * toEffector;
			toGoal				= worldRot.inverse() * toGoal;
			quat4f localRotated	= quat4f::s_fromTo(toEffector, toGoal);
#endif
			_ikChains[j].rotation = localRotated * _ikChains[j].rotation;	// apply localRotate in current rotation (local)

			if (_constraintsHandler != nullptr) {
				_constraintsHandler(j, this);
			}

			effector = getGlobalTransform(last).position;
			if ((goal - effector).lenSq() < thresholdSq) {
				return true;
			}
		}
	}
	return false;
}

}
