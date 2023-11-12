#pragma once

#include <sge_game_anime_prog/math/Transform.h>
#include <sge_game_anime_prog/math/dual_quat.h>

namespace sge {
/*
	The Pose class is used to hold the transformation of every bone in an animated hierarchy.
	Think of it as a frame in an animation;
	the Pose class represents the state of an animation at a given time.
*/

// The Pose class needs to keep track of the transformation of every joint in the skeleton of the character that you are animating.
// It also needs to keep track of the parent joint of every joint.
	// This data is kept in two parallel vectors. --> '_jointTrans' and '_parentIds'
	// In this context a joint is expressed as a Transform object. --> '_jointTrans[i]'
class Pose {
public:
	inline Pose() = default;
	inline Pose(size_t jointCount) { resize(jointCount); }

	Pose(const Pose& r);
	Pose& operator=(const Pose& r); // overload operator=, use std::memcpy for quick copy

	bool operator== (const Pose& r) const;
	bool operator!= (const Pose& r) const;

	void clear();
	void resize(size_t newJointCount);

	inline size_t getJointCount() const { return _jointTrans.size(); }

	inline int	getParent(int i) const	{ return _parentIds[i]; }
	inline void setParent(int i, int p)	{ _parentIds[i] = p; }

	inline Transform	getLocalTransform(int i) const					{ return _jointTrans[i]; }
	inline void			setLocalTransform(int i, const Transform& t)	{ _jointTrans[i] = t; }

	Transform	getGlobalTransform(int i) const;
	dual_quat4f getGlobalDualQuaternion(int i) const;

	inline			Transform getWorldTransform(int i)	const { return getGlobalTransform(i); }
	inline const	Transform operator[] (int i)		const { return getGlobalTransform(i); }

	inline dual_quat4f getWorldDualQuaternion(int i)	const { return getGlobalDualQuaternion(i); }

	// For a Pose class to be passed to OpenGL, it needs to be converted into a linear array of matrices.
	// The function takes a reference to a vector of matrices
	// and fills it out with the global transform matrix of each joint in the pose
	void getMatrixPalette(Vector<mat4f>& out) const;
	void getDualQuaternionPalette(Vector<dual_quat4f>& out) const;

private:
	// To store a parent-child hierarchy between transforms, you need to maintain two parallel vectors
	// one filled with transforms and one filled with integers.
	// The integer array contains the index of the parent transform for each joint.
	Vector<Transform>	_jointTrans;
	Vector<int>			_parentIds;
};


}