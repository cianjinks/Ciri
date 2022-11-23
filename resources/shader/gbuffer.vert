#version 460 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelMatrix;
uniform mat3 u_NormalMatrix;

out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(a_Pos, 1.0);
    v_Normal = normalize(u_NormalMatrix * a_Normal); // normalize(vec3(u_ModelMatrix * vec4(a_Normal, 0.0f)));
    v_TexCoord = a_TexCoord;
}