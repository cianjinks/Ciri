#ifndef CIRI_MATH_H
#define CIRI_MATH_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Ciri
{
    namespace Math
    {
        class Transform
        {
        public:
            Transform() = default;
            ~Transform() = default;
            Transform(const Transform &other) = default;
            Transform(const glm::mat4 &mat);

            void Update();

            void SetParentMatrix(const glm::mat4 &mat);
            void SetLocalMatrix(const glm::mat4 &mat);
            void SetLocalTranslation(const glm::vec3 &translation);
            void SetLocalRotation(const glm::vec3 &rotation);
            void SetLocalScale(const glm::vec3 &scale);

            const glm::mat4 &GetLocalMatrix();
            const glm::mat4 &GetWorldMatrix();

            const glm::vec3 &Transform::GetLocalTranslation() const
            {
                return m_LocalTranslation;
            }

            const glm::vec3 &Transform::GetLocalScale() const
            {
                return m_LocalScale;
            }

            const glm::quat &Transform::GetLocalRotation() const
            {
                return m_LocalRotation;
            }

        private:
            glm::vec3 m_LocalTranslation = glm::vec3(0.0f);
            glm::quat m_LocalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 m_LocalScale = glm::vec3(1.0f);

            glm::mat4 m_LocalMatrix = glm::mat4(1.0f);
            glm::mat4 m_WorldMatrix = glm::mat4(1.0f);
            glm::mat4 m_ParentMatrix = glm::mat4(1.0f);

            bool m_Dirty = false;
        };

        glm::mat4 ComposeTransform(const glm::vec3 &translation, const glm::quat &rotation, const glm::vec3 &scale);
        glm::mat4 ComposeTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale);
        bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &r_translation, glm::vec3 &r_rotation, glm::vec3 &r_scale);
        bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &r_translation, glm::quat &r_rotation, glm::vec3 &r_scale);
    }
}

#endif