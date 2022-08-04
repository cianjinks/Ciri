#ifndef CIRI_GLTF_H
#define CIRI_GLTF_H

#include "Scene/Scene.h"

namespace Ciri
{
    class GLTFImporter
    {
    public:
        static S<SceneNode> Import(Scene* scene, const char *filepath);
    };
}

#endif