#pragma once
#include <sge_game_anime_prog/math/Transform.h>

namespace sge{
/*
	CCDSolver class with three variables:
		-a list of transforms to form the IK chain, --> _ikChain
		-the number of iterations to perform, --> _stepCount
		-and some small delta --> _threshold
			that can be used to control how close the target has to be to the goal before the chain is considered solved.
*/
class CCDSolver : public NonCopyable {
public:
	CCDSolver() = default;

	inline size_t getJointCount() const { return _ikChain.size(); }
	inline void resize(size_t newSize) { _ikChain.resize(newSize); }

	inline       Transform& operator[](int i)		{ return _ikChain[i]; }
	inline const Transform& operator[](int i) const	{ return _ikChain[i]; }

	Transform getGlobalTransform(int i) const;

	inline int stepCount() const		{ return _stepCount; }
	inline void setStepCount(int sc)	{ _stepCount = sc; }

	inline float threshold() const		{ return _threshold; }
	inline float setThreshold(float th) { _threshold = th; }

	bool solve(const Transform& target);

private:
	// Assume that the IK chain has a parent-child relationship
	// where every index is the child of the index before it, making 0 our root node.
	// As such, every transform in the IK chain is declared in local space.
	Vector<Transform> _ikChain;
	int _stepCount   = 15; // The _stepCount variable is used to make sure the solver doesn't fall into an infinite loop.
	float _threshold = 0.00001f;
};

}