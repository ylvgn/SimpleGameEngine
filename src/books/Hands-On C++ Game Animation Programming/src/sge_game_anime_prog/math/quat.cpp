#include "quat.h"
#include "mat4.h"

namespace sge {

void quat::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "quat({}, {}, {}, {})", x, y, z, w);
}

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

		vec3 ortho = vec3::s_right();	// x-axis is the most orthogonal axis of f vector

		if (Math::abs(f.y) < Math::abs(f.x)) {
			ortho = vec3::s_up();		// y-axis is the most orthogonal axis of f vector
		}

		// when from vector is close to XOY-plane
		if (Math::abs(f.z) < Math::abs(f.y) && Math::abs(f.z) < Math::abs(f.x)) {
			ortho = vec3::s_forward(); // z-axis is the most orthogonal axis of f vector
		}

		vec3 axis = f.cross(ortho).normalize();
		return quat(axis.x, axis.y, axis.z, 0);
	}

	vec3 half = (f + t).normalize(); // θ/2 between f and t
	vec3 axis = f.cross(half);
	return quat(axis.x, axis.y, axis.z, f.dot(half));
//	The quat::s_fromTo function is one of the most intuitive ways of creating a quaternion.
}

quat quat::s_lookRotation(const vec3& dir, const vec3& up) {
	vec3 f			= dir.normalize();		// object space forward vector
	vec3 desiredUp	= up.normalize();		// desired up vector
	vec3 r			= desiredUp.cross(f);	// object space right vector

	desiredUp		= f.cross(r);			// object space up vector(incorrect)

	quat f2d		= quat::s_fromTo(vec3::s_forward(), f);// From world forward to object forward
	vec3 objectUp	= f2d * vec3::s_up();					// what direction is the new object up?
	quat u2u		= quat::s_fromTo(objectUp, desiredUp);	// From object up to desired up

	quat result(f2d * u2u);
	return result.normalize();
/*
	The mat4::s_lookAt function creates a view matrix, which is the inverse of the camera's transformation.
	This means the rotation of mat4::s_lookAt and the result of quat::s_lookRotation are going to be the inverse of each other.
	So, quat::s_mat4(mat4::s_lookAt(camera.pos, aim.pos, camera.up)) == inverse(quat::s_lookRotation(aim.pos-camera.pos, camera.up))
*/
}

quat quat::s_mat4(const mat4& m) {
	// FYI: https://gabormakesgames.com/blog_quats_to_matrix.html#:~:text=%7D-,From%20Matrix,-Similar%20to%20how
	// Using only the forward and up vectors,
	// the quat::s_lookRotation function can be used to convert a matrix into a quaternion.

	vec3 u = m.up().xyz().normalize();
	vec3 f = m.forward().xyz().normalize();
	vec3 r = u.cross(f);
	u = f.cross(r);
	return quat::s_lookRotation(f, u);
/*
	Converting matrices to quaternions is going to be useful for debugging
	and in the case where an external data source only provides rotations as matrices.
*/
}

}