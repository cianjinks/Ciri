#ifndef CIRI_MATH_H
#define CIRI_MATH_H

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Ciri
{
    namespace Math
    {
        /* TODO: Add rotation. */
        bool DecomposeTransform(const glm::mat4& transform, glm::vec3& r_translation, glm::vec3& r_scale);
    }
}

#endif