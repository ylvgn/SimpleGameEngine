#pragma once

#include <sge_game_anime_prog/math/Transform.h>

namespace sge {

class FABRIKSolver : public NonCopyable {
public:

	inline       Transform& operator[](int i)		{ return _ikChains[i]; }
	inline const Transform& operator[](int i) const { return _ikChains[i]; }

	inline void setLocalTransform(int i, const Transform& t)	{ _ikChains[i] = t; }
	inline Transform getLocalTransform(int i) const				{ return _ikChains[i]; }

	Transform getGlobalTransform(int i) const;
	inline Transform getWorldTransform(int i) const { return getGlobalTransform(i); }

	inline void setStepCount(int stepCount)			{ _stepCount = stepCount; }
	inline int	stepCount() const					{ return _stepCount; }

	inline void setThreshold(float threshold)		{ _threshold = threshold; }
	inline float threshold() const					{ return _threshold; }

	inline size_t getJointCount() const				{ return _ikChains.size(); }
	void resize(size_t newJointCount);

	bool solve(const Transform& target);

private:
	void _ikChainsToWorld();
	void _worldToIkChains();

	void _iterateBackward(const vec3f& goal);
	void _iterateForward (const vec3f& base);

	Vector<Transform>	_ikChains;
	Vector<vec3f>		_worldChians;
	Vector<float>		_lengths;

	int _stepCount		= 15;
	float _threshold	= 0.00001f;
};

}