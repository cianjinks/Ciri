#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_DepthTexture;
uniform sampler2D u_BaseColorTexture;
uniform sampler2D u_NormalOcclusionTexture;

uniform mat4 u_InvProjMat;
uniform mat4 u_InvViewMat;

struct PointLight
{
    vec3 position;
    vec3 color;
};
#define MAX_POINT_LIGHTS 256
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_NumPointLights;

vec3 ComputeWorldSpacePosition(float depth)
{
    vec4 clipSpacePosition = vec4(v_TexCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePosition = u_InvProjMat * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = u_InvViewMat * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 CalcPointLight(PointLight light, vec3 basecolor, vec3 normal, vec3 fragpos)
{
    vec3 lightDir = normalize(light.position - fragpos);

    vec3 ambient = 0.1 * basecolor;
    vec3 diffuse = light.color * max(dot(normal, lightDir), 0.0) * basecolor;
    return (ambient + diffuse);
}

void main()
{
    float Depth = texture(u_DepthTexture, v_TexCoord).x;
    vec3 FragPos = ComputeWorldSpacePosition(Depth);
    vec3 BaseColor = texture(u_BaseColorTexture, v_TexCoord).xyz;
    vec3 Normal = texture(u_NormalOcclusionTexture, v_TexCoord).xyz;

    vec3 result = vec3(0.0);
    for (int p = 0; p < u_NumPointLights; p++)
    {
        result += CalcPointLight(u_PointLights[p], BaseColor, Normal, FragPos);
    }
    a_Color = vec4(result, 1.0f);
}