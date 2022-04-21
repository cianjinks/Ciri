#version 460 core

layout(location = 0) out vec4 a_Pos;
layout(location = 1) out vec3 a_Normal;

in vec4 v_Pos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_BaseColor;

void main()
{
    a_Pos = v_Pos;
    a_Normal = v_Normal;
}