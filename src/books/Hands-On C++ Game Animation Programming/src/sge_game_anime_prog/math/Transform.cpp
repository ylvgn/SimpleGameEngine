#include "Transform.h"
#include "mat4.h"

namespace sge {

Transform Transform::mix(const Transform& r, float t) const {
	quat rRot = r.rotation;
	if (rotation.dot(rRot) < 0.0f) { // neighborhood
		rRot = -rRot;
	}

	return Transform(position.lerp(r.position, t),
		             rotation.nlerp(rRot, t),
		             scale.lerp(r.scale, t)
	);
}

Transform Transform::inverse() const {
	Transform inv;

	float sx, sy, sz;
	sx = sy = sz = 0.f;

	inv.rotation = rotation.inverse();

	// When inverting scale, keep in mind that 0 can't be inverted.
	// The case where scale is 0 will need to be treated specially
	if (!Math::equals0(scale.x)) { sx = 1.f / scale.x; }
	if (!Math::equals0(scale.y)) { sy = 1.f / scale.y; }
	if (!Math::equals0(scale.z)) { sz = 1.f / scale.z; }
	inv.scale = vec3f(sx,sy,sz);

	vec3f invTranslation = -position;
	inv.position = inv.rotation * (inv.scale * invTranslation); // first, apply the scale, then rotation, and finally, the translation

	return inv;
}

Transform Transform::s_combine(const Transform& a, const Transform& b) {
/*
	transform combination is equivalent to matrix combination.
		ex1: SRT = Mtranslate  * Mrotation * Mscale
		ex2: MVP = Mprojection * Mview     * Mmodel

	matrix order: scale->rotation->translate
	transform order: scale->rotation->position

	now Transform is pos(vec3f), rotate(quat4f), scale(vec3f), familiar to mat4f*mat4f theory
		- lhs(pos1, rotate1, scale1)
		- rhs(pos2, rotate2, scale2)
			- scale2*scale1 == scale1*scale2
			- rotate2*rotate1 --> apply rotate1 first
			- pos1 + (rotate1*scale1*pos2) --> kind of pos2 change space, which same as pos1's space
*/
	Transform res;

	res.scale    = a.scale    * b.scale;	// same as b.scale * a.scale
	res.rotation = b.rotation * a.rotation; // apply a.rotation first, and then b.rotation

//	The combined position needs to be affected by the rotation and scale components as well.
//	When finding the combined position, remember the order of transformations:
//		scale first, rotate second, and translate last.
	res.position = a.rotation * (a.scale * b.position);
	res.position = a.position + res.position;

	return res;
}

Transform Transform::s_mat(const mat4& m) {
/*
	It's important that you're able to convert matrices to transforms
	because you don't always control what format the data you are dealing with comes in.
	For example, a model format might store matrices instead of transforms.
*/

	Transform res;
	res.position = vec3f(m.v[12], m.v[13], m.v[14]); // wx, wy, wz, same as m.cw().xyz()
	res.rotation = quat::s_mat4(m);

	// To find the scale, first, ignore the translation part of the matrix,
	// M(zero out the translation vector).This leaves you with M = SR.
	mat4f rotScaleMat(m.v[0], m.v[1], m.v[2],  0,
		              m.v[4], m.v[5], m.v[6],  0,
		              m.v[8], m.v[9], m.v[10], 0,
		              0,      0,      0,       1);

	mat4f invRotMat = mat4f::s_quat(res.rotation.inverse());

	// The result would leave a matrix that contains a scale and some skew information.
	mat4f scaleSkewMat = rotScaleMat * invRotMat;

	// simply take the main diagonal as the scale-skew matrix.
	// While this will work most of the time, it's not perfect.
	// The scale that is acquired should be considered a lossy scale,
	// as the value can contain skew data as well, which makes the scale inaccurate.
	res.scale = vec3f(scaleSkewMat.v[0],
		              scaleSkewMat.v[5],
		              scaleSkewMat.v[10]); // lossy scale

	return res;
/*
	It is possible to decompose a matrix into translation, rotation, scale, skew, and the sign of the determinant.
	However, this decomposition is expensive and not well suited to real-time applications.
	FYI: https://gabormakesgames.com/blog_decomposition.html
*/
}

}