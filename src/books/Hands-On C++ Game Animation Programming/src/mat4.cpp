#include "mat4.h"

namespace sge {

// FYI: https://gabormakesgames.com/blog_quats_to_matrix.html#:~:text=%7D-,From%20Matrix,-Similar%20to%20how
quat mat4::s_quat(const mat4& m) {
	// Using only the forward and up vectors,
	// the quat::s_lookRotation function can be used to convert a matrix into a quaternion.

	vec3 up = m.up.xyz().normalize();
	vec3 forward = m.forward.xyz().normalize();
	vec3 right = up.cross(forward);
	up = forward.cross(right);
	return quat::s_lookRotation(forward, up);

	/*
	Converting matrices to quaternions is going to be useful for debugging
	and in the case where an external data source only provides rotations as matrices.
	*/
}

}