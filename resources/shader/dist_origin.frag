#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_DepthTexture;

uniform mat4 u_InvProjMat;
uniform mat4 u_InvViewMat;

uniform float u_CameraFar;
uniform float u_CameraNear;

vec3 ComputeWorldSpacePosition(float depth)
{
    vec4 clipSpacePosition = vec4(v_TexCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePosition = u_InvProjMat * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = u_InvViewMat * viewSpacePosition;

    return worldSpacePosition.xyz;
}

void main()
{
    float depth = texture(u_DepthTexture, v_TexCoord).x;
    vec3 ws_position = ComputeWorldSpacePosition(depth);
    float dist_origin = distance(ws_position, vec3(0.0f, 0.0f, 0.0f)) / u_CameraFar;
    a_Color = vec4(vec3(dist_origin), 1.0);
}