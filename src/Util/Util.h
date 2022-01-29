#ifndef CIRI_UTIL_H
#define CIRI_UTIL_H

#include <glm/glm.hpp>

namespace Ciri
{
    class Util
    {
    public:
        // Math
        static glm::vec3 spherical_to_cartesian(float radius, float theta, float phi);
        static glm::vec3 calc_tri_surface_normal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    };
}

#endif