#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_NormalOcclusionTexture;

void main()
{
    a_Color = vec4(abs(texture(u_NormalOcclusionTexture, v_TexCoord).xyz), 1.0f);
}