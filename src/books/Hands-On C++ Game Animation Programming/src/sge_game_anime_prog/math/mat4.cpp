#include "mat4.h"
#include "Transform.h"
#include "dual_quat.h"

namespace sge {

mat4 mat4::inverse() const {
	// inverse(M) = adjugate(M)/determinant(M)

	// Not all matrices have an inverse. Only matrices with a non-zero determinant can be inverted.
		// ex1: the view matrix that is used to transform three-dimensional objects
			// to be displayed on screen is the inverse of the camera's position and rotation.
		// ex2: Mesh Skinning.

	// Finding the inverse of a matrix is rather complicated as it needs other support functions
	// such as transpose and adjugate

	float det = determinant();
	if (Math::equals0(det)) { // Epsilon check would need to be REALLY small
		throw SGE_ERROR("Trying to invert a matrix with a zero determinant\n");
	}

	auto reciprocal = 1.0f / det;
	return adjugate() * reciprocal;
}

mat4 mat4::inverse3x3() const {
	// inverse3x3(M) = adjugate3x3(M)/determinant3x3(M)

	auto det = determinant3x3();
	auto reciprocal = 1.0f / det;

	return mat4(
		// x basis vector
		(yy*zz-yz*zy) *  reciprocal, // xx
		(xy*zz-xz*zy) * -reciprocal, // xy
		(xy*yz-xz*yy) *  reciprocal, // xz
		0,							 // xw
		// y basis vector
		(yx*zz-zx*yz) * -reciprocal, // yx
		(xx*zz-xz*zx) *  reciprocal, // yy
		(xx*yz-xz*yx) * -reciprocal, // yz
		0,							 // yw
		// z basis vector
		(yx*zy-zx*yy) *  reciprocal, // zx
		(xx*zy-xy*zx) * -reciprocal, // zy
		(xx*yy-xy*yx) *  reciprocal, // zz
		0,							 // zw
		// w basis vector
		0,0,0,1
	);
/*
	Inverting matrices is a relatively expensive function.
	Matrices that only encode the position and rotation can be inverted faster
	because the inverse of a 3x3 rotation matrix is the same as its transpose.
*/
}

void mat4::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "mat4(\n  {},\n  {},\n  {},\n  {})", cx(), cy(), cz(), cw());
}

mat4 mat4::s_lookAt(const vec3& eye, const vec3& aim, const vec3& up) {
		// FYI: http://www.songho.ca/opengl/gl_camera.html#lookat
		// The eye position and target are defined in world space.
		// The rest of the work is finding the inverted basis vectors and figuring out where the position is.

		// finding the inverted basis vectors: rotating the scene with reverse orientation
/*		view matrix = Mrotation*Mtranslation
		            = [r0, r4, r8,  0]*[1, 0, 0, tx]=[r0, r4, r8,  r0*tx + r4*ty + r8*tz ]
			          [r1, r5, r9,  0] [0, 1, 0, ty] [r1, r5, r9,  r1*tx + r5*ty + r9*tz ]
			          [r2, r6, r10, 0] [0, 0, 1, tz] [r2, r6, r10, r2*tx + r6*ty + r10*tz]
			          [0,  0,  0,   1] [0, 0, 0, 1 ] [0,  0,  0,   1                     ]
*/
		vec3 f = (aim - eye).normalize();
		vec3 r = f.cross(up).normalize();
		if (r.equals0()) {
			return mat4::s_identity();
		}

		vec3 u = r.cross(f);

		// Since the basis vectors are orthonormal, their inverse is the same as their transpose.
		// Remember, forward is negative z
		// Transpose upper 3x3 matrix to invert it
		// figuring out where the position is: translating the whole scene inversely from the eye position to the origin
		// The position can be calculated by negating the dot product of the position column vector with the inverted basis vectors.
		return mat4( r.x,         u.x,        -f.x,        0,
			         r.y,         u.y,        -f.y,        0,
			         r.z,         u.z,        -f.z,        0,
			        -r.dot(eye), -u.dot(eye),  f.dot(eye), 1);
/*
		mat4::s_lookAt is equivalent to view matrix
		The view matrix is the inverse of the camera's transformation (the position, rotation, and scale of the camera).

		Instead of having to create the camera's transform matrix and then invert it,
		you will be implementing a mat4::s_lookAt function that generates this matrix directly.

		The mat4::s_lookAt function is the most convenient way of constructing a view matrix.
		example like: mat4::s_lookAt(camera.position, {0,0,0}, {0,1,0})
*/
}

mat4 mat4::s_translate(const vec3& t) {
	return mat4(1,	 0,	  0,   0,
				0,	 1,	  0,   0,
				0,	 0,   1,   0,
				t.x, t.y, t.z, 1);
}

#if 0 // is it the following algorithm wrong ???
mat4 mat4::s_quat(const quat& q) {
	// FYI: https://gabormakesgames.com/blog_quats_to_matrix.html
	// It's less efficient, but easier to understand quaternion to matrix conversion if we think about basis vectors. 
	// there is bug, because positive angle measures represent a counterclockwise rotation,
	vec3 r = q * vec3::s_right();
	vec3 u = q * vec3::s_up();
	vec3 f = q * vec3::s_forward();

	return mat4(r.x, r.y, r.z, 0,  // x basis vector (No scale)
				u.x, u.y, u.z, 0,  // y basis vector (No scale)
				f.x, f.y, f.z, 0,  // z basis vector (No scale)
				0,   0,   0,   1); // w basis vector (No scale)
	}
#else
mat4 mat4::s_quat(const quat& q) {
#if 0
	// Chapter 6 of <Graphics Gems II> implement by Ken Shoemake
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

	return mat4(ww+xx-yy-zz, 2*xy+2*wz,   2*xz-2*wy,   0,
		        2*xy-2*wz,   ww-xx+yy-zz, 2*yz+2*wx,   0,
		        2*xz+2*wy,   2*yz-2*wx,   ww-xx-yy+zz, 0,
		        0,           0,           0,           ww+xx+yy+zz);
#else
	// since q is a unit quaternion, so ww+xx+yy+zz=1
	// FYI: http://www.songho.ca/opengl/gl_quaternion.html
	float xx = q.x*q.x;
	float yy = q.y*q.y;
	float zz = q.z*q.z;
	float wx = q.w*q.x;
	float wy = q.w*q.y;
	float wz = q.w*q.z;
	float xy = q.x*q.y;
	float xz = q.x*q.z;
	float yz = q.y*q.z;

	return mat4(1-2*(yy+zz),    2*(xy+wz),		2*(xz-wy),     0,
		        2*(xy-wz),      1-2*(xx+zz),	2*(yz+wx),     0,
		        2*(xz+wy),      2*(yz-wx),		1-2*(xx+yy),   0,
		        0,              0,				0,             1);
#endif
}
#endif

mat4 mat4::s_scale(const vec3& s) {
	return mat4(s.x, 0,   0,   0,
				0,   s.y, 0,   0,
				0,   0,   s.z, 0,
				0,   0,   0,   1);
}

mat4 mat4::s_trs(const vec3& t, const quat& r, const vec3& s) {
	// M = SRT, so apply scale first, then rotate, last is translate
	return s_translate(t) * s_quat(r) * s_scale(s);
}

mat4 mat4::s_transform(const Transform& t) {
#if 0
	// First, extract the rotation basis of the transform
	vec3 r = t.rotation * vec3::s_right();
	vec3 u = t.rotation * vec3::s_up();
	vec3 f = t.rotation * vec3::s_forward();

	// Next, scale the basis vectors
	r = r * t.scale.x;
	u = u * t.scale.y;
	f = f * t.scale.z;

	return mat4(r.x,          r.y,          r.z,          0, // X basis (& scale)
				u.x,          u.y,          u.z,          0, // Y basis (& scale)
				f.x,          f.y,          f.z,          0, // Z basis (& scale)
				t.position.x, t.position.y, t.position.z, 1  // Position
	);
#else	
	return mat4::s_trs(t.position, t.rotation, t.scale);
#endif
}

mat4 mat4::s_dual_quat(const dual_quat& dq) {
	// dual-quaternions can be converted to and from matrices as well;
	// however, that operation 'mat4::s_dual_quat' is generally not used.

	// result = mat4::s_trs(dq.toTranslation(), dq.toRotation(), vec3f::s_one());

	auto n   = dq.normalize();
	auto q   = n.toRotation();
	auto t   = n.toTranslation();

	float xx = q.x*q.x;
	float yy = q.y*q.y;
	float zz = q.z*q.z;
	float wx = q.w*q.x;
	float wy = q.w*q.y;
	float wz = q.w*q.z;
	float xy = q.x*q.y;
	float xz = q.x*q.z;
	float yz = q.y*q.z;

	return mat4(1-2*(yy+zz),	2*(xy+wz),		2*(xz-wy),		0,
		        2*(xy-wz),      1-2*(xx+zz),	2*(yz+wx),		0,
		        2*(xz+wy),      2*(yz-wx),		1-2*(xx+yy),	0,
				t.x,			t.y,			t.z,			1);
}

}