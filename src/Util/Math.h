#ifndef CIRI_MATH_H
#define CIRI_MATH_H

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Ciri
{
    namespace Math
    {
        bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &r_translation, glm::vec3 &r_rotation, glm::vec3 &r_scale);
        glm::mat4 ComposeTransform(glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

        static inline glm::mat4 AssimpConvertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
            glm::mat4 to;
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
            return to;
        }
    }
}

#endif