#ifndef CIRI_ASSIMP_H
#define CIRI_ASSIMP_H

#include "Scene/Scene.h"

namespace Ciri
{
    class AssimpImporter
    {
    public:
        static S<SceneNode> Import(Scene* scene, const char *filepath);
    };
}

#endif