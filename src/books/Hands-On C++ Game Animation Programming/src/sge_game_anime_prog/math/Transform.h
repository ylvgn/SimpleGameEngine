#pragma once

#include "vec3.h"
#include "quat.h"
#include "mat4.h"

namespace sge {

/*
	The reason you need an explicit Transform struct is that matrices don't interpolate well.
	Interpolating transforms is very important for animation. 
	It's how you create in-between poses to display two given keyframes.
*/

struct Transform {
	vec3 position {0,0,0};
	quat rotation {0,0,0,1};
	vec3 scale {1,1,1};

	Transform() = default;
	Transform(const vec3& p, const quat& r, const vec3& s) : position(p), rotation(r), scale(s) { }

	inline bool operator==(const Transform& r) const { return position == r.position && rotation == r.rotation && scale == r.scale; }
	inline bool operator!=(const Transform& r) const { return !(this->operator==(r)); }

	// Instead of interpolation, this operation is typically called blend or mix.
	// Being able to mix transformations together is important for creating smooth transitions between animations.
	Transform mix(const Transform& r, float t) const;

	Transform inverse() const;

	// same as multiplying a matrix and a point, but just step by step (SRT) in function transformPoint.
	inline vec3 transformPoint(const vec3& pos) {
		// first, apply the scale, then rotation, and finally, the translation
		return position + (rotation * (scale * pos));
//		transformPoint aka move points
	}

	// same as multiplying a matrix and a vector, just like transformPoint but no need add position
	inline vec3 transformVector(const vec3& dir) {
		// first, apply the scale, then rotation, and finally, the translation, but don't add the position
		return rotation * (scale * dir);
//		transformVector aka rotate vectors
	}

	// Consider a skeleton, as an example. At each joint, you could place a transform to describe the motion of the joint.
	// When you rotate your shoulder, the elbow attached to that shoulder also moves.
	// To apply the shoulder transformation to all connected joints, the transform on each joint must be combined with its parent joint's transform.
	static Transform s_combine(const Transform& a, const Transform& b);

	static mat4 s_mat4(const Transform& t);
};

Transform g_mat4ToTransform(const mat4& m);

}