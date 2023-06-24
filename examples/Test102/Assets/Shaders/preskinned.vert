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
uniform mat4 animated[120]; // pre-multiplied skin matrix

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

void main() {
	mat4 m0 = animated[joints.x];
	mat4 m1 = animated[joints.y];
	mat4 m2 = animated[joints.z];
	mat4 m3 = animated[joints.w];

	mat4 skin = (m0*weights.x) + (m1*weights.y) + (m2*weights.z) + (m3*weights.w);

	vec4 skinnedPos    = skin * vec4(position, 1.0);
	vec4 skinnedNormal = skin * vec4(normal, 0.0);
	
	gl_Position = projection * view * model * skinnedPos;
	
	fragPos = vec3(model * skinnedPos);
	norm = vec3(model * skinnedNormal);
    uv = texCoord;
}



