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

    float linear;
    float quadratic;
};
#define MAX_POINT_LIGHTS 64 // Setting this or MAX_SPOT_LIGHTS to 128 breaks the shader. No error is given but clearly there is a limit on uniform array sizes.
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_NumPointLights;

struct SpotLight
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;

    float innerCutoff;
    float outerCutoff;
};
#define MAX_SPOT_LIGHTS 64
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
uniform int u_NumSpotLights;

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
    // Ambient
    vec3 ambient = light.ambient * basecolor; // Material model has no ambient cause it was designed for PBR, use basecolor instead

    // Diffuse
    vec3 lightDir = normalize(light.position - fragpos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * basecolor;

    // Specular
    vec3 viewDir = normalize(camerapos - fragpos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = light.specular * spec; // Material model has specular, but it is not output as part of the gbuffer

    // Attenuation
    float distance = length(light.position - fragpos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));
    // ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 basecolor, vec3 normal, vec3 camerapos, vec3 fragpos)
{
    // Ambient
    vec3 ambient = light.ambient * basecolor;

    // Spotlight Intensity
    vec3 lightDir = normalize(light.position - fragpos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * basecolor;

    // Specular
    vec3 viewDir = normalize(camerapos - fragpos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = light.specular * spec;

    // Attenuation
    float distance = length(light.position - fragpos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));
    // ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

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

    for (int p = 0; p < u_NumSpotLights; p++)
    {
        result += CalcSpotLight(u_SpotLights[p], BaseColor, Normal, u_CameraPos, FragPos);
    }

    a_Color = vec4(result, 1.0f);
}