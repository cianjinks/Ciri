#ifndef CIRI_COMPONENT_H
#define CIRI_COMPONENT_H

#include "Mesh/Mesh.h"

#include <glm/glm.hpp>

namespace Ciri
{
    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &) = default;
        TagComponent(const std::string &tag)
            : Tag(tag) {}
    };

    struct TransformComponent
    {
        Math::Transform Transform;

        TransformComponent() = default;
        TransformComponent(const TransformComponent &other) = default;
        TransformComponent(Math::Transform transform)
            : Transform(transform) {}
        TransformComponent(const glm::mat4 &mat)
            : Transform(mat) {}
    };

    struct MeshComponent
    {
        S<Mesh> CMesh;

        MeshComponent() = default;
        MeshComponent(const MeshComponent &other) = default;
        MeshComponent(const S<Mesh> &mesh)
            : CMesh(mesh) {}
    };
}

#endif