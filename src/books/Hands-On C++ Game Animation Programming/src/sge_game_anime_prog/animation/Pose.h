#pragma once

#include <sge_game_anime_prog/math/Transform.h>

/*
	The Pose class is used to hold the transformation of every bone in an animated hierarchy.
	Think of it as a frame in an animation;
	the Pose class represents the state of an animation at a given time.
*/

/*
	The Pose class needs to keep track of the transformation of every joint in the skeleton of the character that you are animating.
	It also needs to keep track of the parent joint of every joint.
	This data is kept in two parallel vectors.

	In this context a joint is expressed as a Transform object.
	jointCount aka boneCount
*/

namespace sge {

class Pose {
public:
	inline Pose() = default;
	inline Pose(size_t jointCount) { resize(jointCount); };

	inline Pose(const Pose& r) { *this = r; };
	Pose& operator=(const Pose& r);

	// Since the parent and joint vectors are parallel, the resize function needs to set the size of both.
	inline void resize(size_t jointCount) {
		_parents.resize(jointCount);
		_joints.resize(jointCount);
		// at initialization, joint's parent is self
	}

	inline size_t size() const { return _joints.size(); }

	inline int getParent(int i) const   { return _parents[i]; }
	inline void setParent(int i, int p) { _parents[i] = p; }

	inline Transform getLocalTransform(int i) const          { return _joints[i]; }
	inline void setLocalTransform(int i, const Transform& t) { _joints[i] = t; }

	Transform getGlobalTransform(int i) const;

	      Transform operator[] (int i)       { return getGlobalTransform(i); };
	const Transform operator[] (int i) const { return getGlobalTransform(i); };

	// For a Pose class to be passed to OpenGL, it needs to be converted into a linear array of matrices.
	// The function takes a reference to a vector of matricesand fills it out with the global transform matrix of each joint in the pose
	void getMatrixPalette(Vector<mat4>& out) const;

	bool operator== (const Pose& r) const;
	bool operator!= (const Pose& r) const;

	inline void clear() { resize(0); }

private:
	// To store a parent-child hierarchy between transforms, you need to maintain two parallel vectors
	// one filled with transformsand one filled with integers.
	// The integer array contains the index of the parent transform for each joint.
	Vector<Transform> _joints; // aka bones
	Vector<int> _parents;
};


}