#include "FABRIKSolver.h"

namespace sge {

void FABRIKSolver::_worldToIkChains() {
	size_t jointCount = _ikChains.size();
	if (jointCount == 0) return;

	for (int i = 0; i < jointCount - 1; ++i) {
		Transform world = getGlobalTransform(i);
		Transform next  = getGlobalTransform(i+1);

		const vec3f&  worldPos	= world.position;
		const quat4f& worldRot	= world.rotation;

		vec3f toNext			= next.position - worldPos;
		vec3f toDesired			= _worldChians[i+1] - worldPos;

#if 1 // calc in local space
		toNext				= worldRot.inverse() * toNext;
		toDesired			= worldRot.inverse() * toDesired;
		quat4f localRotated	= quat4f::s_fromTo(toNext, toDesired);
#else // calc in world space
		quat4f nextToDesired	= quat4f::s_fromTo(toNext, toDesired);
		quat4f worldRotated	= worldRot * nextToDesired;
		quat4f localRotated	= worldRotated * worldRot.inverse();
#endif
		_ikChains[i].rotation = localRotated * _ikChains[i].rotation;
	}
}

void FABRIKSolver::_ikChainsToWorld() {
	size_t jointCount = _ikChains.size();
	if (jointCount == 0) return;

	for (int i = 0; i < jointCount; ++i) {
		Transform world = getGlobalTransform(i);
		_worldChians[i] = world.position;

		if (i >= 1) {
			vec3f prev = _worldChians[i - 1];
			_lengths[i] = (_worldChians[i] - prev).len();
		}
	}

	if (jointCount > 0) {
		// The lengths array stores the distance of a joint from its parent.
		// This means that the root joint will always contain the length 0.
		// so the length of "0->1" is _lengths[1]
		_lengths[0] = 0.0f;
	}
}

void FABRIKSolver::_iterateBackward(const vec3f& goal) {
	size_t jointCount = _ikChains.size();
	if (jointCount == 0) return;

	int last = static_cast<int>(jointCount) - 1;
	_worldChians[jointCount - 1] = goal; // sets the last element in the chain to be at the goal.

	for (int i = last - 1; i >= 0; --i) {
		vec3f dir = (_worldChians[i] - _worldChians[i+1]).normalize();
		vec3f offset = dir * _lengths[i+1];
		_worldChians[i] = _worldChians[i+1] + offset;
	}
}

void FABRIKSolver::_iterateForward(const vec3f& base) {
	size_t jointCount = _ikChains.size();
	if (jointCount == 0) return;

	_worldChians[0] = base; // sets the initial joint to be at the base.

	for (int i = 1; i < jointCount; ++i) {
		vec3f dir = (_worldChians[i] - _worldChians[i-1]).normalize();
		vec3f offset = dir * _lengths[i];
		_worldChians[i] = _worldChians[i-1] + offset;
	}
}

void FABRIKSolver::resize(size_t newJointCount) {
	_ikChains.resize(newJointCount);
	_worldChians.resize(newJointCount);
	_lengths.resize(newJointCount); // actually length only need newJointCount - 1, but we need _lengths[0] == 0.f
}

Transform FABRIKSolver::getGlobalTransform(int i) const {
	SGE_ASSERT(i >= 0 && i < _ikChains.size());
	Transform res = _ikChains[i];
	for (int j = i - 1; j >= 0; --j) {
		res = Transform::s_combine(_ikChains[j], res);
	}
	return res;
}

bool FABRIKSolver::solve(const Transform& target) {
	size_t jointCount = _ikChains.size();
	if (jointCount == 0) return false;

	_ikChainsToWorld();

	const auto& goal = target.position;
	const vec3f base = _worldChians[0];

	int last = static_cast<int>(jointCount) - 1;
	float thresholdSq = _threshold * _threshold;

	for (int i = 0; i < _stepCount; ++i) {
		const vec3f& effector = _worldChians[last];
		if ((effector - goal).lenSq() < thresholdSq) {
			_worldToIkChains();
			return true;
		}

		_iterateBackward(goal);
		_iterateForward(base);
	}

	_worldToIkChains();

	const vec3f& effector = _worldChians[last];
	if ((effector - goal).lenSq() < thresholdSq) {
		// Check one last time whether the end effector has reached its goal, and return the appropriate Boolean
		return true;
	}
	return true;
}

}