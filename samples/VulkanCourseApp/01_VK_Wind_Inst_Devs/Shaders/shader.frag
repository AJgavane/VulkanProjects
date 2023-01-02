#version 450

// Interpolated color from vertex (location must match)
layout(location = 0) in vec3 fragColor;


// Output color. Must also have location!
layout(location = 0) out vec4 out_FragColor;

void main(){
	out_FragColor = vec4(fragColor, 1.0);
}