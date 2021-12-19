#version 460 core

layout(location = 0) out vec4 a_Color;

in vec3 v_Normal;
in vec3 v_Color;
in vec2 v_TexCoord;

uniform sampler2D u_DiffuseTexture;

void main()
{
    a_Color = texture(u_DiffuseTexture, v_TexCoord);
}