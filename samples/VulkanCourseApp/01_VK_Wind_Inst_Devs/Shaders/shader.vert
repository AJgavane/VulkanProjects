#version 450

// Output color for vertex (Location is required!)
layout(location = 0) out vec3 fragColor;

// Tri vertex positions (that we will put ino the vertex buffer later!)
vec3 positions[3] = vec3[](
						vec3( 0.0, -0.4, 0.0),
						vec3( 0.4,  0.4, 0.0),
						vec3(-0.4,  0.4, 0.0)
					);

// Tri vertex colors
vec3 colors[3] = vec3[](
						vec3(1.0, 0.0, 0.0),
						vec3(0.0, 1.0, 0.0),
						vec3(0.0, 0.0, 1.0)
					 );



void main() {
	fragColor   = colors[gl_VertexIndex];
	gl_Position = vec4(positions[gl_VertexIndex], 1.0);
}