#version 460 core

in vec3 v_pos;

uniform float u_time;

out vec4 o_color;

void main()
{
    o_color = vec4(-v_pos.y, -v_pos.y, v_pos.y, 1.0);
}
