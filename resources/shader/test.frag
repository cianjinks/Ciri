#version 460 core

layout(location = 0) out vec4 a_Color;

in vec3 v_Normal;

void main()
{
    a_Color = vec4(v_Normal, 1.0);
}