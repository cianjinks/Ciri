#ifndef CIRI_OBJ_H
#define CIRI_OBJ_H

#include "Scene/Scene.h"

namespace Ciri
{
    class OBJImporter
    {
    public:
        static bool Import(Scene* scene, S<SceneNode> container, const char *filepath);
    };
}

#endif