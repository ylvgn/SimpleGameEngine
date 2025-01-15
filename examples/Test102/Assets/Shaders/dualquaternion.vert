#version 330 core

// MVP
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// attributes
in vec3 position;
in vec3 normal;
in vec2 texCoord;

// skinning info
in vec4 weights;
in ivec4 joints;
uniform mat2x4 pose[120]; // 120 is arbitrary, according to jointCount
uniform mat2x4 invBindPose[120];

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

// ------------------------------------------------ qual start
vec4 mulQ(vec4 Q1, vec4 Q2) {
	return vec4(
		 Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		 Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
	);
}

vec3 mulQV(vec4 Q, vec3 v) {
	// nVidia SDK implementation
	vec3 qv  = Q.xyz;
	vec3 uv  = cross(qv, v);
	vec3 uuv = cross(qv, uv);
	return v + (uv * Q.w + uuv) * 2.0;
}
// ------------------------------------------------ qual end

// ------------------------------------------------ dual_qual start
float magnitudeDq(mat2x4 dq) {
	return length(dq[0]);
}

mat2x4 normalizeDq(mat2x4 dq) {
	float m = magnitudeDq(dq);
	float reciprocal = 1.0 / m;
	dq[0] *= m;
	dq[1] *= m;
	return dq;
}

vec4 getDqRotation(mat2x4 dq) {
	return dq[0];
}

vec3 getDqTranslation(mat2x4 dq) {
	vec4 real      = dq[0];
	vec4 dual      = dq[1];
	vec4 conjugate = vec4(-real.xyz, real.w);
	return mulQ(conjugate, dual * 2.0).xyz;
}

vec3 mulDqVector(mat2x4 dq, vec3 vector) {
	vec4 real = dq[0];
	return mulQV(real, vector);
}

vec3 mulDqPoint(mat2x4 dq, vec3 point) {
	// result = dq.real * point + dq.translation
	vec4 real = dq[0];
	return mulQV(real, point) + getDqTranslation(dq);
}

mat2x4 mulDq(mat2x4 l, mat2x4 r) {
	// result = mat2x4(l.real*r.real, (l.real*r.dual) + (l.dual*r.real));
	l = normalizeDq(l);
	r = normalizeDq(r);
	vec4 real = mulQ(l[0], r[0]);
	vec4 dual = mulQ(l[0], r[1]) + mulQ(l[1], r[0]);
	return mat2x4(real, dual);
}
// ------------------------------------------------ dual_qual end


// perform matrix palette skinning, same logic as linear mesh skinning skin matrix
void main() {
#if 0 // linear mesh skinning (src from skinned.vert)
	mat4 m0 = pose[joints.x] * invBindPose[joints.x];
	mat4 m1 = pose[joints.y] * invBindPose[joints.y];
	mat4 m2 = pose[joints.z] * invBindPose[joints.z];
	mat4 m3 = pose[joints.w] * invBindPose[joints.w];

	mat4 skin = (m0*weights.x) + (m1*weights.y) + (m2*weights.z) + (m3*weights.w);

	vec4 skinnedPos    = skin * vec4(position, 1.0);
	vec4 skinnedNormal = skin * vec4(normal, 0.0);
#else // dual quaternion mesh skinning

	vec4 w = weights;

	// Neighborhood all of the quaternions correctly
	if (dot(pose[joints.x][0], pose[joints.y][0]) < 0.0) { w.y *= -1.0; }
	if (dot(pose[joints.x][0], pose[joints.z][0]) < 0.0) { w.z *= -1.0; }
	if (dot(pose[joints.x][0], pose[joints.w][0]) < 0.0) { w.w *= -1.0; }

	// Mul DualQuaternion order is left to right!!
	mat2x4 dq0 = mulDq(invBindPose[joints.x], pose[joints.x]);
	mat2x4 dq1 = mulDq(invBindPose[joints.y], pose[joints.y]);
	mat2x4 dq2 = mulDq(invBindPose[joints.z], pose[joints.z]);
	mat2x4 dq3 = mulDq(invBindPose[joints.w], pose[joints.w]);

	mat2x4 skin = normalizeDq((w.x*dq0) + (w.y*dq1) + (w.z*dq2) + (w.w*dq3));

	vec4 skinnedPos = vec4(mulDqPoint(skin, position), 1);
	vec4 skinnedNormal = vec4(mulDqPoint(skin, normal), 0);
#endif

	gl_Position = projection * view * model * skinnedPos;
	fragPos 	= vec3(model * skinnedPos);
	norm 		= vec3(model * skinnedNormal);
    uv 			= texCoord;
}