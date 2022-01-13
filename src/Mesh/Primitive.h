#ifndef CIRI_PRIMITIVE_H
#define CIRI_PRIMITIVE_H

#include <glm/glm.hpp>

#include "Mesh.h"

namespace Ciri
{
    class Cube : public Mesh
    {
    public:
        Cube(const char *name);
        Cube(const char *name, glm::vec3 color);
    };

    class Quad : public Mesh
    {
    public:
        Quad(const char *name);
        Quad(const char *name, glm::vec3 color);
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