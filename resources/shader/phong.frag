#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_DepthTexture;
uniform sampler2D u_BaseColorTexture;
uniform sampler2D u_NormalOcclusionTexture;

uniform mat4 u_InvProjMat;
uniform mat4 u_InvViewMat;

uniform vec3 u_CameraPos;

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define MAX_POINT_LIGHTS 128 // Setting this to 256 breaks the shader when the PointLight struct is > 9 bytes :/
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

vec3 CalcPointLight(PointLight light, vec3 basecolor, vec3 normal, vec3 camerapos, vec3 fragpos)
{
    vec3 lightDir = normalize(light.position - fragpos);

    vec3 ambient = light.ambient * basecolor; // Material model has no ambient cause it was designed for PBR, use basecolor instead

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * basecolor;

    vec3 viewDir = normalize(camerapos - fragpos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = light.specular * spec; // Material model has specular, but it is not output as part of the gbuffer
    return (ambient + diffuse + specular);
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
        result += CalcPointLight(u_PointLights[p], BaseColor, Normal, u_CameraPos, FragPos);
    }
    a_Color = vec4(result, 1.0f);
}