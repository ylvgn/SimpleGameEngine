#version 330 core

// The fragment shader takes the object's diffused color from a texture,
// then applies a single-directional light.
// The lighting model is just N dot L.
// Because there is no ambient term to the light, some parts of the model can appear as all black

// from vertex shader 
in vec3 norm;
in vec3 fragPos;
in vec2 uv;

// from user input
uniform vec3 light;  
uniform sampler2D tex0;

out vec4 FragColor;

void main() {
	vec4 diffuseColor = texture(tex0, uv);
	
	vec3 n = normalize(norm);
	vec3 l = normalize(light);
	float diffuseIntensity = clamp(dot(n, l) + 0.1, 0, 1); // Intensity is a simple directional light
	
	FragColor = diffuseColor * diffuseIntensity;
}