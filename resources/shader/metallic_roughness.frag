#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_MetallicRoughnessTexture;

void main()
{
    vec2 metallic_roughness = texture(u_MetallicRoughnessTexture, v_TexCoord).gb;
    a_Color = vec4(0.0f, metallic_roughness, 1.0f);
}