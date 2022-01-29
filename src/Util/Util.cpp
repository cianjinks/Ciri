#include "Util.h"

namespace Ciri
{
    glm::vec3 Util::spherical_to_cartesian(float radius, float theta, float phi)
    {
        glm::vec3 result = glm::vec3(0.0f);
        result.x = radius * glm::sin(theta) * glm::cos(phi);
        result.z = radius * glm::sin(theta) * glm::sin(phi);
        result.y = radius * glm::cos(theta);
        return result;
    }

    glm::vec3 Util::calc_tri_surface_normal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
    {
        glm::vec3 u = v2 - v1;
        glm::vec3 v = v3 - v1;

        glm::vec3 normal = glm::vec3(0.0f);
        normal.x = (u.y * v.z) - (u.z * v.y);
        normal.y = (u.z * v.x) - (u.x * v.z);
        normal.z = (u.x * v.y) - (u.y * v.x);
        return normal;
    }
}