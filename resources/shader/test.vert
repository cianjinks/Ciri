#version 460 core

layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec3 a_Col;

uniform mat4 u_MVP;

out vec3 v_Color;

void main()
{
    gl_Position = u_MVP * vec4(a_Pos, 0.0, 1.0);
    v_Color = a_Col;
}