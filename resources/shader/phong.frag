#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_DepthTexture;
uniform sampler2D u_BaseColorTexture;
uniform sampler2D u_NormalOcclusionTexture; // Occlusion is actually Phong Shininess
uniform sampler2D u_MetallicRoughnessTexture; // Actually Phong Ambient
uniform sampler2D u_EmissiveTexture; // Actually Phong Specular

uniform mat4 u_InvProjMat;
uniform mat4 u_InvViewMat;

uniform vec3 u_CameraPos;
uniform float u_CameraFar;
uniform float u_CameraNear;

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

#define GLOBAL_AMBIENT 0.1f
#define ENABLE_FOG 1

vec3 ComputeWorldSpacePosition(float depth)
{
    vec4 clipSpacePosition = vec4(v_TexCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePosition = u_InvProjMat * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = u_InvViewMat * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 CalcPointLight(PointLight light, vec3 basecolor, vec3 mat_ambient, vec3 mat_spec, float mat_shininess, vec3 normal, vec3 camerapos, vec3 fragpos)
{
    // Ambient
    vec3 ambient = light.ambient * mat_ambient; // basecolor * GLOBAL_AMBIENT;

    // Diffuse
    vec3 lightDir = normalize(light.position - fragpos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * basecolor;

    // Specular
    vec3 viewDir = normalize(camerapos - fragpos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), mat_shininess);
    vec3 specular = light.specular * spec * mat_spec;

    // Attenuation
    float distance = length(light.position - fragpos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));
    // ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 basecolor, vec3 mat_ambient, vec3 mat_spec, float mat_shininess, vec3 normal, vec3 camerapos, vec3 fragpos)
{
    // Ambient
    vec3 ambient = light.ambient * mat_ambient; // * basecolor * GLOBAL_AMBIENT;

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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), mat_shininess);
    vec3 specular = light.specular * spec * mat_spec;

    // Attenuation
    float distance = length(light.position - fragpos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));
    // ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

#if ENABLE_FOG
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * u_CameraNear * u_CameraFar) / (u_CameraFar + u_CameraNear - z * (u_CameraFar - u_CameraNear));
}

float GetFogFactor(float d)
{
    const float FogMax = 150.0;
    const float FogMin = 100.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}
#endif

void main()
{
    float Depth = texture(u_DepthTexture, v_TexCoord).x;
    vec3 FragPos = ComputeWorldSpacePosition(Depth);
    vec3 BaseColor = texture(u_BaseColorTexture, v_TexCoord).xyz;
    vec3 Normal = texture(u_NormalOcclusionTexture, v_TexCoord).xyz;
    vec3 PhongAmbient = texture(u_MetallicRoughnessTexture, v_TexCoord).rgb;
    vec3 PhongSpecular = texture(u_EmissiveTexture, v_TexCoord).rgb;
    float PhongShininess = texture(u_NormalOcclusionTexture, v_TexCoord).a;

    vec3 result = vec3(0.0);
    for (int p = 0; p < u_NumPointLights; p++)
    {
        result += CalcPointLight(u_PointLights[p], BaseColor, PhongAmbient, PhongSpecular, PhongShininess, Normal, u_CameraPos, FragPos);
    }

    for (int p = 0; p < u_NumSpotLights; p++)
    {
        result += CalcSpotLight(u_SpotLights[p], BaseColor, PhongAmbient, PhongSpecular, PhongShininess, Normal, u_CameraPos, FragPos);
    }

#if ENABLE_FOG
    vec3 fog_color = vec3(0.1);
    a_Color = vec4(mix(result, fog_color, GetFogFactor(LinearizeDepth(Depth))), 1.0f);
#else
    a_Color = vec4(result, 1.0f);
#endif
}