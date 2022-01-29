#ifndef CIRI_PRIMITIVE_H
#define CIRI_PRIMITIVE_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Mesh.h"

namespace Ciri
{
    class Cube : public Mesh
    {
    public:
        Cube();
    };

    class Quad : public Mesh
    {
    public:
        Quad();
    };

    class Sphere : public Mesh
    {
    public:
        Sphere(uint32_t h_segments = 4, uint32_t v_segments = 4, bool flat = false);
    };

    class Plane : public Mesh
    {
    public:
        Plane();
    };
}

#endif