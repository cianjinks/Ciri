#ifndef CIRI_ANIMATION_H
#define CIRI_ANIMATION_H

#include "Scene/Scene.h"

#include "Bone.h"

namespace Ciri
{
    class Animation
    {
    private:
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        std::map<std::string, BoneInfo> m_BoneInfoMap;

        std::vector<glm::mat4> m_FinalBoneMatrices;
        float m_CurrentTime;
        float m_DeltaTime;

    public:
        Animation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter);
        ~Animation() = default;

        void ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter);

        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration;}
        inline const std::map<std::string,BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

        Bone* FindBone(const std::string& name)
        {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                [&](const Bone& Bone)
                {
                    return Bone.GetBoneName() == name;
                }
            );
            if (iter == m_Bones.end()) return nullptr;
            else return &(*iter);
        }

        void UpdateAnimation(S<SceneNode>& node, float dt);
        void CalculateBoneTransform(S<SceneNode>& node, glm::mat4 parentTransform);
        std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
    };
}

#endif