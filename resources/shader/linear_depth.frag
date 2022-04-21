#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_DepthTexture;

uniform float u_CameraFar;
uniform float u_CameraNear;
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * u_CameraNear * u_CameraFar) / (u_CameraFar + u_CameraNear - z * (u_CameraFar - u_CameraNear));	
}
void main()
{
    float depth = texture(u_DepthTexture, v_TexCoord).x;
    float linear_depth = LinearizeDepth(depth) / u_CameraFar; // divide by far for demonstration
    a_Color = vec4(vec3(linear_depth), 1.0);
}