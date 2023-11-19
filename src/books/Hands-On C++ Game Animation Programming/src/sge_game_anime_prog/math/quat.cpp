#include "quat.h"

#include "mat4.h"

namespace sge {

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