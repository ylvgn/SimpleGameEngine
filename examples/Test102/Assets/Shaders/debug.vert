#version 330 core

uniform mat4 mvp;
in vec3 position;

void main() {
	gl_Position = mvp * vec4(position, 1.0);
};



