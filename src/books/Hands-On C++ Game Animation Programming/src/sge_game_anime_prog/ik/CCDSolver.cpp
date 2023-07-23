#include "CCDSolver.h"

namespace sge {

Transform CCDSolver::getGlobalTransform(int i) const {
	Transform world(_ikChain[i]);
	for (int j = i - 1; j >= 0; --j) {
		world = Transform::s_combine(_ikChain[j], world);
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
	int jointCount = static_cast<int>(_ikChain.size());
	if (jointCount < 2) {
		return false;
	}

	const float thresholdSq  = _threshold * _threshold;
	const vec3& goalWorldPos = target.position;

	int last = jointCount - 1;
	for (int i = 0; i < _stepCount; ++i) {

		vec3 effectorWorldPos = getGlobalTransform(last).position;
		if ((goalWorldPos - effectorWorldPos).lenSq() < thresholdSq) {
			return true;
		}

		for (int j = jointCount - 2; j >= 0; --j) {
			effectorWorldPos = getGlobalTransform(last).position;
			Transform world  = getGlobalTransform(j);
			vec3 worldPos = world.position;
			quat worldRot = world.rotation;

			vec3 toEffectorDir = effectorWorldPos - worldPos;
			vec3 toGoalDir     = goalWorldPos - worldPos;

			quat effectorToGoal = quat::s_identity();
			if (toGoalDir.lenSq() > 0.00001f) {
				effectorToGoal = quat::s_fromTo(toEffectorDir, toGoalDir);
			}

			quat worldRotated    = worldRot  * effectorToGoal;
			quat localRotate     = worldRotated * worldRot.inverse();
			_ikChain[j].rotation = localRotate  * _ikChain[j].rotation;

			effectorWorldPos = getGlobalTransform(last).position;
			if ((goalWorldPos - effectorWorldPos).lenSq() < thresholdSq) {
				return true;
			}
		}
	}
	return false;
}

}
