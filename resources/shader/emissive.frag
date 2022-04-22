#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_EmissiveTexture;

void main()
{
    a_Color = texture(u_EmissiveTexture, v_TexCoord);
}