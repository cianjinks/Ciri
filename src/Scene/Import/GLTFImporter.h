#ifndef CIRI_GLTF_H
#define CIRI_GLTF_H

#include "Scene/Scene.h"

namespace Ciri
{
    class GLTFImporter
    {
    public:
        static bool Import(Scene* scene, S<SceneNode> container, std::string filepath);
    };
}

#endif