#ifndef CIRI_COMPONENT_H
#define CIRI_COMPONENT_H

#include "Mesh/Mesh.h"

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