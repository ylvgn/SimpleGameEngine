#version 330 core

// MVP
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// attributes
in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0);
	
	fragPos = vec3(model * vec4(position, 1.0));
	norm = vec3(model * vec4(normal, 0.0f));
    uv = texCoord;
	
	// This is a minimal vertex shader;
	// it only puts the vertex through a model view and projection pipeline.
	// This shader can be used to display static geometry or CPU skinned meshes.
}



