#ifndef CIRI_PRIMITIVE_H
#define CIRI_PRIMITIVE_H

#include <glm/glm.hpp>

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
        Sphere();
    };

    class Plane : public Mesh
    {
    public:
        Plane();
    };
}

#endif