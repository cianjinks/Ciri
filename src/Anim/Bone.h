#ifndef CIRI_BONE_H
#define CIRI_BONE_H

/* TODO: Quickly hacked this together from: https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation *
/*       Relies on assimp and is not well architected. But it is fine for now. */
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/anim.h>

namespace Ciri
{
    struct BoneInfo
    {
        int id;
        glm::mat4 offset;
    };

    struct KeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

    class Bone
    {
    private:
        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;
        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;

        glm::mat4 m_LocalTransform;
        std::string m_Name;
        int m_ID;

    public:
        Bone(const std::string& name, int ID, const aiNodeAnim* channel);
        void Update(float animationTime);
        glm::mat4 GetLocalTransform() { return m_LocalTransform; }
        std::string GetBoneName() const { return m_Name; }
        int GetBoneID() { return m_ID; }
        int GetPositionIndex(float animationTime);
        int GetRotationIndex(float animationTime);
        int GetScaleIndex(float animationTime);

    private:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        glm::mat4 InterpolatePosition(float animationTime);
        glm::mat4 InterpolateRotation(float animationTime);
        glm::mat4 InterpolateScaling(float animationTime);
    };
}

#endif