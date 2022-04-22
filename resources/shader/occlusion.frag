#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_NormalOcclusionTexture;

void main()
{
    float occlusion = texture(u_NormalOcclusionTexture, v_TexCoord).a;
    a_Color = vec4(vec3(occlusion), 1.0f);
}