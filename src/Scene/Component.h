#ifndef CIRI_COMPONENT_H
#define CIRI_COMPONENT_H

#include "Render/Material.h"
#include "Mesh/Mesh.h"

#include "entt.hpp"
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

    class Entity;

    struct HierarchyComponent
    {
        Entity Parent; /* Parent. */
        Entity First;  /* First Child. */
        Entity Prev;   /* Previous at current level. */
        Entity Next;   /* Next at current level. */

        HierarchyComponent() = default;
        HierarchyComponent(const HierarchyComponent &) = default;
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

    struct MaterialComponent
    {
        S<Material> CMaterial;

        MaterialComponent() = default;
        MaterialComponent(const MaterialComponent &other) = default;
        MaterialComponent(const S<Material> &material)
            : CMaterial(material) {}
    };
}

#endif