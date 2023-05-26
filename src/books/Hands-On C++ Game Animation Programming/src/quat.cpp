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

// FYI: https://gabormakesgames.com/blog_quats_create.html#:~:text=%7D-,Look%20At,-To%20implement%20a
quat quat::s_lookRotation(const vec3& dir, const vec3& up) {
	vec3 f = dir.normalize();	// object space forward vector
	vec3 u = up.normalize();	// desired up vector
	vec3 r = u.cross(f);		// object space right vector
	u = f.cross(r);				// object space up vector(incorrect)

	// From world forward to object forward
	quat f2d = s_fromTo(vec3(0, 0, 1), f);

	// what direction is the new object up?
	vec3 objectUp = f2d * vec3(0, 1, 0);
	// From object up to desired up
	quat u2u = s_fromTo(objectUp, u);

	// Rotate to forward direction first, then twist to correct up.
	// Combine rotations (in reverse! forward applied first, then up)
	quat result = f2d * u2u;

	// should be: quat result = u2u * f2d; ??????????
	return result.normalize();
	/*
	The matrix lookAt function creates a view matrix, which is the inverse of the camera transform.
	This means the rotation of lookAt and the result of lookRotation are going to be the inverse of each other.
	*/
}

mat4 quat::s_mat4(const quat& q) {
	// FYI: https://gabormakesgames.com/blog_quats_to_matrix.html
#if 0
	// It's less efficient, but easier to understand quaternion to matrix conversion if we think about basis vectors. 
	// there is bug, because positive angle measures represent a counterclockwise rotation, but the following is wrong!!?????
	vec3 r = q * vec3::s_right();
	vec3 u = q * vec3::s_up();
	vec3 f = q * vec3::s_forward();

	return mat4(r.x, r.y, r.z, 0,  // x basis vector (No scale)
				u.x, u.y, u.z, 0,  // y basis vector (No scale)
				f.x, f.y, f.z, 0,  // z basis vector (No scale)
				0,   0,   0,   1   // w basis vector (No scale)
	);
#else

#if 0
	float ww = q.w*q.w;
	float xx = q.x*q.x;
	float yy = q.y*q.y;
	float zz = q.z*q.z;
	float wx = q.w*q.x;
	float wy = q.w*q.y;
	float wz = q.w*q.z;
	float xy = q.x*q.y;
	float xz = q.x*q.z;
	float yz = q.y*q.z;

	return Matrix4(ww+xx-yy-zz, 2*xy-2*wz,   2*xz+2*wy,   0,
		           2*xy+2*wz,   ww-xx+yy-zz, 2*yz-2*wx,   0,
		           2*xz-2*wy,   2*yz + 2*wx, ww-xx-yy+zz, 0,
		           0,           0,           0,           ww+xx+yy+zz
	);
#else
	// since q is a unit quaternion, so ww+xx+yy+zz=1
	// FYI: http://www.songho.ca/math/quaternion/quaternion.html#matrix
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;

	return mat4(1.f-2.0f*(yy+zz), 2.f*(xy-wz),     2.f*(xz+wy),     0.f,
		        2.f*(xy+wz),      1.f-2.f*(xx+zz), 2.f*(yz-wx),     0.f,
		        2.f*(xz-wy),      2.f*(yz+wx),     1.f-2.f*(xx+yy), 0.f,
		        0.f,              0.f,             0.f,             1.f
	);
#endif

#endif
	/*
	Being able to convert quaternions to matrices will be useful later when you need to pass rotation data to a shader.
	Shaders don't know what a quaternion is, but they have built-in functionality to deal with matrices.
	*/
}

}