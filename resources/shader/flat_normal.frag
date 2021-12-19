#version 460 core

layout(location = 0) out vec4 a_Color;

in flat vec3 v_Normal;
in vec2 v_TexCoord;

void main()
{
    a_Color = vec4(abs(v_Normal), 1.0f);
}