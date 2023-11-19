#pragma once

#include <sge_game_anime_prog/math/Transform.h>

namespace sge {

// FABRIK: Forward And Backward Reaching Inverse Kinematics
class FABRIKSolver : public NonCopyable {
	using This = FABRIKSolver;
public:

	using ConstraintsHandler = void(*) (int i, This* solver);

	void resize(size_t newJointCount);

	void setLocalTransform(int i, const Transform& t)	{ _ikChains[i] = t; }

			Transform& getLocalTransform(int i)			{ return _ikChains[i]; }
	const	Transform& getLocalTransform(int i) const	{ return _ikChains[i]; }

	Transform getWorldTransform(int i) const;

	void setStepCount(int stepCount)		{ _stepCount = stepCount; }
	int	stepCount() const					{ return _stepCount; }

	void setThreshold(float threshold)		{ _threshold = threshold; }
	float threshold() const					{ return _threshold; }

	size_t getJointCount() const			{ return _ikChains.size(); }

	bool solve(const Transform& target);

	void setConstraintsHandler(void (*handler)(int i, This* solver)) {
		_constraintsHandler = reinterpret_cast<ConstraintsHandler>(handler);
	}

private:
	void _ikChainsToWorld();
	void _worldToIkChains();

	void _iterateBackward(const vec3f& goal);
	void _iterateForward (const vec3f& base);

	Vector<Transform>	_ikChains;
	Vector<vec3f>		_worldChians;
	Vector<float>		_lengths;

	int		_stepCount	= 15;
	float	_threshold	= 0.00001f;

	ConstraintsHandler _constraintsHandler = nullptr;
};

}