#version 460 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in ivec4 a_BoneIDs;
layout(location = 4) in vec4 a_BoneWeights;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelMatrix;
uniform mat3 u_NormalMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_FinalBoneMatrices[MAX_BONES];

out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    mat4 BoneTransform = u_FinalBoneMatrices[a_BoneIDs[0]] * a_BoneWeights[0];
        BoneTransform += u_FinalBoneMatrices[a_BoneIDs[1]] * a_BoneWeights[1];
        BoneTransform += u_FinalBoneMatrices[a_BoneIDs[2]] * a_BoneWeights[2];
        BoneTransform += u_FinalBoneMatrices[a_BoneIDs[3]] * a_BoneWeights[3];

    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * BoneTransform * vec4(a_Pos, 1.0f);
    v_Normal = normalize(u_NormalMatrix * vec3(BoneTransform * vec4(a_Normal, 0.0f)));
    v_TexCoord = a_TexCoord;
}