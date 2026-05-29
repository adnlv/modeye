#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

uniform float u_time;
uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_view;

out vec3 v_color;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1);
    v_color = a_color;
}
