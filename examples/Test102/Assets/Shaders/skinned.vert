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
uniform mat4 pose[120]; // 120 is arbitrary, according to jointCount
uniform mat4 invBindPose[120];

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

// perform matrix palette skinning, same logic as cpu mesh skinning skin matrix
void main() {
	mat4 m0 = pose[joints.x] * invBindPose[joints.x];
	mat4 m1 = pose[joints.y] * invBindPose[joints.y];
	mat4 m2 = pose[joints.z] * invBindPose[joints.z];
	mat4 m3 = pose[joints.w] * invBindPose[joints.w];

	mat4 skin = (m0*weights.x) + (m1*weights.y) + (m2*weights.z) + (m3*weights.w);

	vec4 skinnedPos    = skin * vec4(position, 1.0);
	vec4 skinnedNormal = skin * vec4(normal, 0.0);
	
	gl_Position = projection * view * model * skinnedPos;
	
	fragPos = vec3(model * skinnedPos);
	norm = vec3(model * skinnedNormal);
    uv = texCoord;
}



