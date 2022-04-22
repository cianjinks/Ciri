#version 460 core

layout(location = 0) out vec4 a_BaseColor;
layout(location = 1) out vec4 a_NormalOcclusion;
layout(location = 2) out vec4 a_MetallicRoughness;
layout(location = 3) out vec4 a_Emissive;

uniform vec3 u_BaseColor;
uniform sampler2D u_BaseColorTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetallicRoughnessTexture;
uniform sampler2D u_EmissiveTexture;
uniform sampler2D u_OcclusionTexture;

in vec3 v_Normal;
in vec2 v_TexCoord;

void main()
{
    a_BaseColor = texture(u_BaseColorTexture, v_TexCoord) * vec4(u_BaseColor, 1.0f);
    // No Tanget vectors stored in mesh data yet, so can't use TBN matrix
    // Just set as vertex data normal for now
    a_NormalOcclusion.rgb = vec3(v_Normal); // texture(u_NormalTexture, v_TexCoord).rgb;
    a_NormalOcclusion.a = texture(u_OcclusionTexture, v_TexCoord).r;
    // G = metallic, B = roughness
    a_MetallicRoughness.gb = texture(u_MetallicRoughnessTexture, v_TexCoord).gb;
    a_Emissive = texture(u_EmissiveTexture, v_TexCoord);
}