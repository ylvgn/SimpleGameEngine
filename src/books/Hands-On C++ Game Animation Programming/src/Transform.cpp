#include "Transform.h"

namespace sge {

// To keep things consistent, combining transforms should maintain a right - to - left combination order.
Transform Transform::s_combine(const Transform& a, const Transform& b) {
	Transform out;

//	Combining the scale and rotation of two transforms is simple—multiply them together.
	out.scale = a.scale * b.scale;			// same as b.scale * a.scale
	out.rotation = b.rotation * a.rotation; // apply a.rotation first, and then b.rotation

//	The combined position needs to be affected by the rotation and scale components as well.
//	When finding the combined position, remember the order of transformations:
//		scale first, rotate second, and translate last.
	out.position = a.rotation * (a.scale * b.position);
	out.position = a.position + out.position;

	return out;
}

mat4 Transform::s_mat4(const Transform& t) {
#if 1
	// First, extract the rotation basis of the transform
	vec3 r = t.rotation * vec3::s_right();
	vec3 u = t.rotation * vec3::s_up();
	vec3 f = t.rotation * vec3::s_forward();

	// Next, scale the basis vectors
	r = r * t.scale.x;
	u = u * t.scale.y;
	f = f * t.scale.z;

	return mat4(
		r.x,          r.y,          r.z,          0, // X basis (& scale)
		u.x,          u.y,          u.z,          0, // Y basis (& scale)
		f.x,          f.y,          f.z,          0, // Z basis (& scale)
		t.position.x, t.position.y, t.position.z, 1  // Position
	);
#else

	mat4 translate = mat4(1,            0,            0,            0,
				          0,            1,            0,            0,
				          0,            0,            1,            0,
				          t.position.x, t.position.y, t.position.z, 1);

	mat4 rotate = quat::s_mat4(t.rotation);

	mat4 scale  = mat4(t.scale.x, 0,         0,         0,
				       0,         t.scale.y, 0,         0,
				       0,         0,         t.scale.z, 0,
				       0,         0,         0,         1);

	// M = SRT, so apply scale first, then rotate, last is translate
	return translate * rotate * scale;
#endif
}

Transform g_mat4ToTransform(const mat4& m) {
	/*
	It's important that you're able to convert matrices to transforms
	because you don't always control what format the data you are dealing with comes in.
	For example, a model format might store matrices instead of transforms.
	*/

	Transform out;
	//out.position = vec3(m.v[12], m.v[13], m.v[14]);
	out.position = m.cw.xyz(); //wx, wy, wz
	out.rotation = mat4::s_quat(m);

	// To find the scale, first, ignore the translation part of the matrix,
	// M(zero out the translation vector).This leaves you with M = SR.
	mat4 rotScaleMat(
		m.v[0], m.v[1], m.v[2],  0,
		m.v[4], m.v[5], m.v[6],  0,
		m.v[8], m.v[9], m.v[10], 0,
		0,      0,      0,       1
	);

	mat4 invRotMat = quat::s_mat4(out.rotation.inverse());

	// The result would leave a matrix that contains a scale and some skew information.
	mat4 scaleSkewMat = rotScaleMat * invRotMat;

	// simply take the main diagonal as the scale-skew matrix.
	// While this will work most of the time, it's not perfect.
	// The scale that is acquired should be considered a lossy scale,
	// as the value can contain skew data as well, which makes the scale inaccurate.
	out.scale = vec3(scaleSkewMat.v[0],
		             scaleSkewMat.v[5],
		             scaleSkewMat.v[10]); // lossy scale

	return out;
	/*
	It is possible to decompose a matrix into translation, rotation, scale, skew, and the sign of the determinant.
	However, this decomposition is expensive and not well suited to real-time applications.
	FYI: https://research.cs.wisc.edu/graphics/Courses/838-s2002/Papers/polar-decomp.pdf
	*/
}

}