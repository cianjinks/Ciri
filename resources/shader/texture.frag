#version 460 core

layout(location = 0) out vec4 a_Color;

in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_DiffuseTexture;

void main()
{
    // a_Color = vec4(v_TexCoord, 0.0f, 1.0f);
    a_Color = texture(u_DiffuseTexture, v_TexCoord);
}