#version 450

// frag color
layout(location = 0) in vec3 v_color;

// Output color. Must also have location!
layout(location = 0) out vec4 out_FragColor;

void main(){
	out_FragColor = vec4(v_color, 1.0);
}