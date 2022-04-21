#version 460 core

layout(location = 0) out vec3 a_Normal;

in vec3 v_Normal;

void main()
{
    a_Normal = v_Normal;
}