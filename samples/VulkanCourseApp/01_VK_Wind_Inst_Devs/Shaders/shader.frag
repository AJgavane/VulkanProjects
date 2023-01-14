#version 450

// Output color. Must also have location!
layout(location = 0) out vec4 out_FragColor;

void main(){
	out_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}