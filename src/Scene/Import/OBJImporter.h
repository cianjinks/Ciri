#ifndef CIRI_OBJ_H
#define CIRI_OBJ_H

#include "Scene/Scene.h"

namespace Ciri
{
    class OBJImporter
    {
    public:
        static S<SceneNode> Import(Scene* scene, const char *filepath);
    };
}

#endif