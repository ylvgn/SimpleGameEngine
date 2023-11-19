#pragma once

#include <sge_game_anime_prog/math/Transform.h>

namespace sge{
/*
	CCDSolver class with three variables:
		-a list of transforms to form the IK chain, --> _ikChains
		-the number of iterations to perform,		--> _stepCount
		-and some small delta						--> _threshold
			that can be used to control how close the target has to be to the goal before the chain is considered solved.
*/

// CCD: Cyclic Coordinate Descent
class CCDSolver : public NonCopyable {
	using This = CCDSolver;
public:
	using ConstraintsHandler = void(*) (int i, This* solver);

	void setLocalTransform(int i, const Transform& t)	{ _ikChains[i] = t; }

			Transform& getLocalTransform(int i)			{ return _ikChains[i]; }
	const	Transform& getLocalTransform(int i) const	{ return _ikChains[i]; }

	Transform getWorldTransform(int i) const;

	void setStepCount(int stepCount)					{ _stepCount = stepCount; }
	int  stepCount() const								{ return _stepCount; }

	float setThreshold(float threshold)					{ _threshold = threshold; }
	float threshold() const								{ return _threshold; }

	size_t getJointCount() const						{ return _ikChains.size(); }
	void resize(size_t jointCount)						{ _ikChains.resize(jointCount); }

	bool solve(const Transform& target);

	void setConstraintsHandler(void (*handler)(int i, This* solver) ) {
		_constraintsHandler = reinterpret_cast<ConstraintsHandler>(handler);
	}

private:
	// Assume that the IK chain has a parent-child relationship
	// where every index is the child of the index before it, making 0 our root node.
	// As such, every transform in the IK chain is declared in local space.
	Vector<Transform>	_ikChains;
	int					_stepCount	= 15; // The _stepCount variable is used to make sure the solver doesn't fall into an infinite loop.
	float				_threshold	= 0.00001f;

	ConstraintsHandler	_constraintsHandler = nullptr;
};

}