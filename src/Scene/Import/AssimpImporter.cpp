#include "AssimpImporter.h"

namespace Ciri
{
    S<SceneNode> AssimpImporter::Import(Scene *scene, const char *filepath)
    {
        CIRI_LOG("Test assimp import!");
        return scene->AddContainer("Test");
    }
}