#ifndef CIRI_MATH_H
#define CIRI_MATH_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Ciri
{
    namespace Math
    {
        struct Transform
        {
            glm::vec3 Translation = glm::vec3(0.0f);
            glm::vec3 Rotation = glm::vec3(0.0f);
            glm::vec3 Scale = glm::vec3(1.0f);

            Transform() = default;
            ~Transform() = default;
            Transform(const Transform &other) = default;
            Transform(const glm::mat4 &mat);

            glm::mat4 Compose();
            void Decompose(const glm::mat4 &mat);
        };

        glm::mat4 ComposeTransform(const glm::vec3 &translation, const glm::quat &rotation, const glm::vec3 &scale);
        glm::mat4 ComposeTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale);
        bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &r_translation, glm::vec3 &r_rotation, glm::vec3 &r_scale);
    }
}

#endif