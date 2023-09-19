#pragma once

#include "vec3.h"
#include "quat.h"

namespace sge {

/*
	The reason you need an explicit Transform struct is that matrices don't interpolate well.
	Interpolating transforms is very important for animation. 
	It's how you create in-between poses to display two given keyframes.
*/

struct mat4;

struct Transform {
	vec3f  position {0,0,0};
	quat4f rotation {0,0,0,1}; // do not set {0,0,0,0}, it means quat::s_identity
	vec3f  scale	{1,1,1};

	inline Transform() = default;
	inline Transform(const vec3f& p, const quat4f& r, const vec3f& s)
		: position(p), rotation(r), scale(s) {}

	inline bool operator==(const Transform& r) const { return position == r.position && rotation == r.rotation && scale == r.scale; }
	inline bool operator!=(const Transform& r) const { return !this->operator==(r); }

	// Instead of interpolation, this operation is typically called blend or mix.
	// Being able to mix transformations together is important for creating smooth transitions between animations.
	Transform mix(const Transform& r, float t) const;
	Transform lerp(const Transform& r, float t) const { return mix(r, t); }

	Transform inverse() const;

	inline vec3f forward()	const { return rotation * vec3f::s_forward(); }
	inline vec3f right()	const { return rotation * vec3f::s_right(); }
	inline vec3f up()		const { return rotation * vec3f::s_up(); }

	// same as multiplying a matrix and a point, but just step by step (SRT) in function transformPoint.
	inline vec3f transformPoint(const vec3f& pos) const {
		// first, apply the scale, then rotation, and finally, the translation
		return position + (rotation * (scale * pos));
//		transformPoint aka move points
	}

	// same as multiplying a matrix and a vector, just like transformPoint but no need add position
	inline vec3f transformVector(const vec3f& dir) const {
		// first, apply the scale, then rotation, and finally, the translation, but don't add the position
		return rotation * (scale * dir);
//		transformVector aka rotate vectors
	}

	// Consider a skeleton, as an example. At each joint, you could place a transform to describe the motion of the joint.
	// When you rotate your shoulder, the elbow attached to that shoulder also moves.
	// To apply the shoulder transformation to all connected joints, the transform on each joint must be combined with its parent joint's transform.
	// To keep things consistent, combining transforms should maintain a right-to- left combination order.
	static Transform s_combine(const Transform& a, const Transform& b);
	static Transform s_mat(const mat4& m);
};

}