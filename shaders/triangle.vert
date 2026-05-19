#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

uniform float u_time;

out vec3 v_color;

void main()
{
    gl_Position = vec4(a_pos, 1.0);
    gl_Position.y += sin(u_time);

    v_color = a_color;
}
