#version 460 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in ivec4 a_BoneIDs;
layout(location = 4) in vec4 a_BoneWeights;

uniform mat4 u_MVP;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_FinalBoneMatrices[MAX_BONES];

out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(a_BoneIDs[i] == -1)
            continue;
        if(a_BoneIDs[i] >= MAX_BONES)
        {
            totalPosition = vec4(a_Pos,1.0f);
            break;
        }
        vec4 localPosition = u_FinalBoneMatrices[a_BoneIDs[i]] * vec4(a_Pos, 1.0f);
        totalPosition += localPosition * a_BoneWeights[i];
    }

    gl_Position = u_MVP * totalPosition;
    v_Normal = a_Normal; // TODO: This is incorrect, we need to transform the normals as above?
    v_TexCoord = a_TexCoord;
}