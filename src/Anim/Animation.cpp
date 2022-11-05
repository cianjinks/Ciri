#include "Animation.h"

namespace Ciri
{
    Animation::Animation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter)
    {
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        ReadMissingBones(animation, boneinfomap, bonecounter);

        m_CurrentTime = 0.0f;
        m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
    }

    void Animation::ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter)
    {
        int size = animation->mNumChannels;
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneinfomap.find(boneName) == boneinfomap.end())
            {
                boneinfomap[boneName].id = bonecounter;
                bonecounter++;
            }
            m_Bones.push_back(Bone(channel->mNodeName.data,
                boneinfomap[channel->mNodeName.data].id, channel));
        }

        m_BoneInfoMap = boneinfomap;
    }

    void Animation::UpdateAnimation(S<SceneNode>& node, float dt)
    {
        m_DeltaTime = dt;
        m_CurrentTime += m_TicksPerSecond * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_Duration);
        CalculateBoneTransform(node, glm::mat4(1.0f));
    }

    void Animation::CalculateBoneTransform(S<SceneNode>& node, glm::mat4 parentTransform)
    {
        std::string nodeName = node->Name;
        glm::mat4 nodeTransform = Math::ComposeTransform(node->Position, node->Rotation, node->Scale);

        Bone* Bone = FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (S<SceneNode> child : node->Children)
        {
            CalculateBoneTransform(child, globalTransformation);
        }
    }
}