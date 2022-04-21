#version 460 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoord;

uniform sampler2D u_PositionTexture;
uniform sampler2D u_NormalTexture;

void main()
{
    // I don't really have anything to do with the current targets together, 
    // so just output position for now
    a_Color = texture(u_PositionTexture, v_TexCoord);
}