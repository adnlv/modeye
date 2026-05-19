#version 460 core

uniform float u_time;

out vec4 o_color;

void main()
{
	float wave = sin(u_time) * 0.5 + 0.5;
	o_color.gb = vec2(0.0);
	o_color.r = wave;
	o_color.a = 1.0;
}
