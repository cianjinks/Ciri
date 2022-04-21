#version 460 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_MVP;

out vec3 v_Normal;

void main()
{
    gl_Position = u_MVP * vec4(a_Pos, 1.0);
    v_Normal = a_Normal;
}