#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_EmissiveTexture;

void main()
{
    // Replaced with phong specular for assignment
    vec3 specular = texture(u_EmissiveTexture, v_TexCoord).rgb;
    a_Color = vec4(specular, 1.0f);
}