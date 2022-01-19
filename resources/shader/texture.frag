#version 460 core

layout(location = 0) out vec4 a_Color;

in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_BaseColor;
uniform sampler2D u_BaseColorTexture;

void main()
{
    a_Color = texture(u_BaseColorTexture, v_TexCoord) * vec4(u_BaseColor, 1.0f);
}