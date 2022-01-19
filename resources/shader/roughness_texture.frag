#version 460 core

layout(location = 0) out vec4 a_Color;

in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_RoughnessTexture;

void main()
{
    a_Color = texture(u_RoughnessTexture, v_TexCoord);
}