#version 450

// Input vertex
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

layout(binding = 0) uniform MVP {
	mat4 projection;
	mat4 view;
	mat4 model;
} mvp;

layout(location = 0) out vec3 v_color;

void main() {
	v_color = a_color;
	gl_Position = mvp.projection * mvp.view * mvp.model * vec4(a_position, 1.0);
}