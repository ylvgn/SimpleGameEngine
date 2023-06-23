#include "quat.h"
#include "mat4.h"

namespace sge {

quat quat::s_fromTo(const vec3& from, const vec3& to) {
/*
	There is one edge case—what happens when v1 and v2 are parallel? Or if v1== -v2?
	The cross product that's used to find the axis of rotation would yield a 0 vector.
	If this edge case happens, find the most perpendicular vector between the two vectors to create a pure quaternion.
*/
	vec3 f = from.normalize();
	vec3 t = to.normalize();

	if (f.equals(t)) {
		return quat::s_identity(); // no rotation
	}
	else if (f.equals(-t)) { // 180 degree rotation
		// when opposites of each other, the most orthogonal axis of the from vector can be used to create a pure quaternion

		vec3 ortho = vec3::s_right(); // x-axis is the most orthogonal axis of f vector

		if (fabsf(f.y) < fabsf(f.x)) {
			ortho = vec3::s_up(); // y-axis is the most orthogonal axis of f vector
		}

		// when from vector is close to XOY-plane
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
			ortho = vec3::s_forward(); // z-axis is the most orthogonal axis of f vector
		}

		vec3 axis = f.cross(ortho).normalize();
		return quat(axis.x, axis.y, axis.z, 0);
	}

	vec3 half = (f + t).normalize(); // θ/2 between f and t
	vec3 axis = f.cross(half);
	return quat(axis.x, axis.y, axis.z, f.dot(half));
//	The s_fromTo function is one of the most intuitive ways of creating a quaternion.
}

quat quat::s_lookRotation(const vec3& dir, const vec3& up) {
	// FYI: https://gabormakesgames.com/blog_quats_create.html#:~:text=%7D-,Look%20At,-To%20implement%20a

	vec3 f = dir.normalize();	// object space forward vector
	vec3 u = up.normalize();	// desired up vector
	vec3 r = u.cross(f);		// object space right vector
	u = f.cross(r);				// object space up vector(incorrect)

	// From world forward to object forward
	quat f2d = s_fromTo(vec3::s_forward(), f);

	// what direction is the new object up?
	vec3 objectUp = f2d * vec3::s_up();

	// From object up to desired up
	quat u2u = s_fromTo(objectUp, u);

	// Rotate to forward direction first, then twist to correct up.
	// Combine rotations (in reverse! forward applied first, then up)
	quat result = f2d * u2u;

	// should be: quat result = u2u * f2d; why ??????????
	return result.normalize();
/*
	The matrix lookAt function creates a view matrix, which is the inverse of the camera transform.
	This means the rotation of lookAt and the result of lookRotation are going to be the inverse of each other.
*/
}

quat quat::s_mat4(const mat4& m) {
	// FYI: https://gabormakesgames.com/blog_quats_to_matrix.html#:~:text=%7D-,From%20Matrix,-Similar%20to%20how
	// Using only the forward and up vectors,
	// the quat::s_lookRotation function can be used to convert a matrix into a quaternion.

	vec3 up		 = m.up().xyz().normalize();
	vec3 forward = m.forward().xyz().normalize();
	vec3 right	 = up.cross(forward);
	up = forward.cross(right);
	return quat::s_lookRotation(forward, up);
/*
	Converting matrices to quaternions is going to be useful for debugging
	and in the case where an external data source only provides rotations as matrices.
*/
}

void quat::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "({},{},{},{})", x,y,z,w);
}

}