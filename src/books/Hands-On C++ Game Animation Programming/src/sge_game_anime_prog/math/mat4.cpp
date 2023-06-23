#include "mat4.h"

namespace sge {

mat4 mat4::inverse() const {
	// Not all matrices have an inverse. Only matrices with a non-zero determinant can be inverted.
	// ex1: the view matrix that is used to transform three - dimensional objects to be displayed on - screen is the inverse of the camera's position and rotation.
	// ex2: Mesh Skinning.

	// Finding the inverse of a matrix is rather complicated as it needs other support functions(such as transpose and adjugate).
	// inverse(M) = adjugate(M)/determinant(M)
	float det = determinant();
	if (Math::equals0(det)) { // Epsilon check would need to be REALLY small
		throw SGE_ERROR("Trying to invert a matrix with a zero determinant\n");
	}

	float oneOverDet = 1.0f / det;
	return adjugate() * oneOverDet;
}

mat4 mat4::inverse3x3() const {
	// inverse3x3(M)=adjugate3x3(M)/determinant3x3(M)
	float det = determinant3x3();
	float oneOverDet = 1.0f / det;

	return mat4(
		// x basis vector
		(yy*zz-yz*zy) * oneOverDet,		// xx
		(xy*zz-xz*zy) * -oneOverDet,	// xy
		(xy*yz-xz*yy) * oneOverDet,		// xz
		0,								// xw
		// y basis vector
		(yx*zz-zx*yz) * -oneOverDet,	// yx
		(xx*zz-xz*zx) * oneOverDet,		// yy
		(xx*yz-xz*yx) * -oneOverDet,	// yz
		0,								// yw
		// z basis vector
		(yx*zy-zx*yy) * oneOverDet,		// zx
		(xx*zy-xy*zx) * -oneOverDet,	// zy
		(xx*yy-xy*yx) * oneOverDet,		// zz
		0,								// zw
		// w basis vector
		0,0,0,1
	);
/*
	Inverting matrices is a relatively expensive function.
	Matrices that only encode the position and rotation can be inverted faster
	because the inverse of a 3x3 rotation matrix is the same as its transpose.
*/
}

mat4 mat4::s_quat(const quat& q) {
#if 0
	// FYI: https://gabormakesgames.com/blog_quats_to_matrix.html
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
	float ww = q.w * q.w;
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;

	return mat4(ww+xx-yy-zz, 2*xy+2*wz,   2*xz-2*wy,   0,
		        2*xy-2*wz,   ww-xx+yy-zz, 2*yz+2*wx,   0,
		        2*xz+2*wy,   2*yz-2*wx,   ww-xx-yy+zz, 0,
		        0,           0,           0,           ww+xx+yy+zz
	);
#else
	// since q is a unit quaternion, so ww+xx+yy+zz=1
	// FYI: http://www.songho.ca/opengl/gl_quaternion.html
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;

	return mat4(1-2*(yy+zz),    2*(xy+wz),     2*(xz-wy),     0,
		        2*(xy-wz),      1-2*(xx+zz),   2*(yz+wx),     0,
		        2*(xz+wy),      2*(yz-wx),     1-2*(xx+yy),   0,
		        0,              0,             0,             1
	);
#endif
#endif
/*
	Being able to convert quaternions to matrices will be useful later when you need to pass rotation data to a shader.
	Shaders don't know what a quaternion is, but they have built-in functionality to deal with matrices.
*/
}

void mat4::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{}\n{}\n{}\n{}", cx(), cy(), cz(), cw());
}

}