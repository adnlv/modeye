#version 460 core

layout (location = 0) in vec3 a_pos;

uniform float u_time;

out vec3 v_pos;

void main()
{
    v_pos = a_pos;
    v_pos.y += sin(u_time);

    gl_Position = vec4(v_pos, 1.0);
}
