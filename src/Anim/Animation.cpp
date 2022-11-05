#include "Animation.h"

namespace Ciri
{
    Animation::Animation(const aiScene* scene, const aiAnimation* animation, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter)
    {
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        ReadHierarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, boneinfomap, bonecounter);

        m_CurrentTime = 0.0f;
        m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
    }

    void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* node)
    {
        dest.name = node->mName.data;
        dest.transformation = Math::AssimpConvertMatrixToGLMFormat(node->mTransformation);
        dest.childrenCount = node->mNumChildren;

        for (int i = 0; i < node->mNumChildren; i++)
        {
            AssimpNodeData newData;
            ReadHierarchyData(newData, node->mChildren[i]);
            dest.children.push_back(newData);
        }
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

    void Animation::UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        m_CurrentTime += m_TicksPerSecond * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_Duration);
        CalculateBoneTransform(&m_RootNode, glm::mat4(1.0f));
    }

    void Animation::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

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

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}