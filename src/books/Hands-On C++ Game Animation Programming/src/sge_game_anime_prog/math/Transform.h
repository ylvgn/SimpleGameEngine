#pragma once

#include "mat4.h"

namespace sge {
/*
	The reason you need an explicit Transform struct is that matrices don't interpolate well.
	Interpolating transforms is very important for animation. 
	It's how you create in-between poses to display two given keyframes.
*/

struct dual_quat;

struct Transform {
public:
	Transform() = default;
	Transform(const vec3f& t, const quat4f& r, const vec3f& s)
		: position(t)
		, rotation(r)
		, scale(s)
	{}

	bool operator==(const Transform& r) const { return position == r.position && rotation == r.rotation && scale == r.scale; }
	bool operator!=(const Transform& r) const { return !this->operator==(r); }

	// Instead of interpolation, this operation is typically called blend or mix.
	// Being able to mix transformations together is important for creating smooth transitions between animations.
	Transform mix (const Transform& r, float t) const;
	Transform lerp(const Transform& r, float t) const { return mix(r, t); }

	Transform inverse() const;

	vec3f forward	()	const { return rotation * vec3f::s_forward(); }
	vec3f right		()	const { return rotation * vec3f::s_right(); }
	vec3f up		()	const { return rotation * vec3f::s_up(); }

	// same as multiplying a matrix and a point, but just step by step (SRT) in function transformPoint.
	vec3f transformPoint(const vec3f& point) const {
		// first, apply the scale, then rotation, and finally, the translation
		return position + (rotation * (scale * point));
//		transformPoint aka move points
	}

	// same as multiplying a matrix and a vector, just like transformPoint but no need add position
	vec3f transformVector(const vec3f& vector) const {
		// first, apply the scale, then rotation, and finally, the translation, but don't add the position
		return rotation * (scale * vector);
//		transformVector aka rotate vectors
	}

	void onFormat(fmt::format_context& ctx) const;

	// Consider a skeleton, as an example. At each joint, you could place a transform to describe the motion of the joint.
	// When you rotate your shoulder, the elbow attached to that shoulder also moves.
	// To apply the shoulder transformation to all connected joints, the transform on each joint must be combined with its parent joint's transform.
	// To keep things consistent, combining transforms should maintain a right-to- left combination order.
	static Transform s_combine(const Transform& a, const Transform& b);
	static Transform s_mat(const mat4f& m);
	static Transform s_dual_quat(const dual_quat& dq);

	vec3f  position {0,0,0};
	quat4f rotation {0,0,0,1}; // do not set '{0,0,0,0}', cuz '{0,0,0,1}' it means quat4f::s_identity
	vec3f  scale	{1,1,1};
};

SGE_FORMATTER(Transform)

}