#version 330 core

#define MAX_BONES 60
#define MAX_INSTANCES 80

// MVP
uniform mat4 view;
uniform mat4 projection;
// uniform mat4 model; use uniform arrays model_pos[], model_rot[], model_scl[] instead

// unique to each actor in the crowd.
uniform vec3 model_pos[MAX_INSTANCES];
uniform vec4 model_rot[MAX_INSTANCES];
uniform vec3 model_scl[MAX_INSTANCES];

uniform ivec2 frames[MAX_INSTANCES];	// x: currentFrame y: nextFrame
uniform float time[MAX_INSTANCES];		// blend time, between currentFrame with nextFrame

// attributes
in vec3 position;
in vec3 normal;
in vec2 texCoord;
// skinning info
in vec4 weights;
in ivec4 joints;
//uniform mat4 pose[MAX_BONES]; // use sampler2D instead
uniform sampler2D animTex;
uniform mat4 invBindPose[MAX_BONES];

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

// similar to `quat::operator* (vec3 v)`
vec3 QMulV(vec4 q, vec3 v) {
	vec3 qv = q.xyz;
	float w = q.w;

	vec3 uv  = cross(qv, v);
	vec3 uuv = cross(qv, uv);
	return v + (uv * w + uuv) * 2;
}

mat4 MTranslate(vec3 t) {
	return mat4(1,	 0,	  0,   0,
				0,	 1,	  0,   0,
				0,	 0,   1,   0,
				t.x, t.y, t.z, 1);
}

mat4 MRotation(vec4 q) {
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
}

mat4 MScale(vec3 s) {
	return mat4(s.x, 0,   0,   0,
				0,   s.y, 0,   0,
				0,   0,   s.z, 0,
				0,   0,   0,   1);
}

mat4 MTRS(vec3 t, vec4 r, vec3 s) {
	return MTranslate(t) * MRotation(r) * MScale(s);
}

// similar to `mat4::s_transform(Transform t)`
mat4 GetModelMatrix(int instanceId) { // built-in glsl variable, gl_InstanceID.
	vec3 t = model_pos[instanceId];
	vec4 r = model_rot[instanceId];
	vec3 s = model_scl[instanceId];
#if 0	
	vec3 r = QMulV(r, vec3(s.x,	0,	 0	))
	vec3 u = QMulV(r, vec3(0,	s.y, 0	))
	vec3 f = QMulV(r, vec3(0,	0,	 s.z))

	return mat4(r.x, r.y, r.z, 0.0, // X basis (& scale)
				u.x, u.y, u.z, 0.0, // Y basis (& scale)
				f.x, f.y, f.z, 0.0, // Z basis (& scale)
				t.x, t.y, t.z, 1.0  // Position
	);
#else
	return MTRS(t, r, s);
#endif
}

// return the animated world matrix of the joint.
mat4 GetPose(int jointId, int instanceId) {
	// uses an animation texture to construct the animated pose

	// finding the x and y positions to sample the animation texture
	int x_current 	= frames[instanceId].x;
	int x_next 		= frames[instanceId].y;

	int y_pos = jointId * 3;

	// use texelFetch instead of `glTexImage2D`
		// cuz `glTexImage2D` expect normalized uv
		// `texelFetch` return the raw data
	// syntax: texelFetch(sampler, xy-coordinates, mipmap level)

	// sample the current frame
	vec4 pos0 = texelFetch(animTex, ivec2(x_current, y_pos + 0), 0);
	vec4 rot0 = texelFetch(animTex, ivec2(x_current, y_pos + 1), 0);
	vec4 scl0 = texelFetch(animTex, ivec2(x_current, y_pos + 2), 0);

	// sample the next frame
	vec4 pos1 = texelFetch(animTex, ivec2(x_next, y_pos + 0), 0);
	vec4 rot1 = texelFetch(animTex, ivec2(x_next, y_pos + 1), 0);
	vec4 scl1 = texelFetch(animTex, ivec2(x_next, y_pos + 2), 0);

	// interpolate from current frme to next frame

	if (dot(rot0, rot1) < 0.0) { // neighborhood
		rot1 *= -1.0;
	}

	vec4 postion	= mix(pos0, pos1, time[instanceId]);
	vec4 rotation	= mix(rot0, rot1, time[instanceId]);
	vec4 scale		= mix(scl0, scl1, time[instanceId]);

	return MTRS(postion.xyz, rotation, scale.xyz);
}

void main() {
	// by finding all four of the animated pose matrices,
	// as well as the model matrix for the current actor in the crowd.
	// Use gl_InstanceID to get the ID of the currently drawn actor

	mat4 pose0 = GetPose(joints.x, gl_InstanceID);
	mat4 pose1 = GetPose(joints.y, gl_InstanceID);
	mat4 pose2 = GetPose(joints.z, gl_InstanceID);
	mat4 pose3 = GetPose(joints.w, gl_InstanceID);

	// same logic as linear mesh skinning skin matrix
	mat4 m0 = pose0 * invBindPose[joints.x];
	mat4 m1 = pose1 * invBindPose[joints.y];
	mat4 m2 = pose2 * invBindPose[joints.z];
	mat4 m3 = pose3 * invBindPose[joints.w];
	
	mat4 skin = (m0*weights.x) + (m1*weights.y) + (m2*weights.z) + (m3*weights.w);

	vec4 skinnedPos    = skin * vec4(position, 1.0);
	vec4 skinnedNormal = skin * vec4(normal,   0.0);

	mat4 model = GetModelMatrix(gl_InstanceID);
	
	gl_Position = projection * view * model * skinnedPos;
	fragPos 	= vec3(model * skinnedPos);
	norm 		= vec3(model * skinnedNormal);
    uv 			= texCoord;
}