#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_NormalTexture;

void main()
{
    a_Color = abs(texture(u_NormalTexture, v_TexCoord));
}