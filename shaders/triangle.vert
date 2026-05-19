#version 460 core

layout (location = 0) in vec3 a_pos;

uniform float u_time;

void main()
{
	vec3 pos = a_pos;
	pos.y += sin(u_time);

	gl_Position = vec4(pos, 1.0);
}
