#ifndef CIRI_ASSIMP_H
#define CIRI_ASSIMP_H

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Ciri
{
    class AssimpImporter
    {
    public:
        static void Import(const S<Scene> &scene, Entity entity, const std::string &path);

    private:
        static void ProcessAssimpNode(const S<Scene> &scene, Entity entity, const aiScene *assimp_scene, const aiNode *node, std::string &file_dir);
        static void ProcessAssimpMesh(const S<Scene> &scene, Entity entity, const aiScene *assimp_scene, const aiMesh *assimp_mesh, std::string &file_dir);
        static void ProcessAssimpMaterial(const S<Scene> &scene, Entity entity, const aiMaterial *assimp_material, std::string &file_dir);
    };

#if 0
    class AssimpImporter
    {
    public:
        static S<SceneNode> Import(Scene* scene, const char *filepath);

    private:
        static void ProcessAssimpMesh(Scene* scene, const S<SceneNode>& container, const aiScene* assimp_scene, const aiMesh* assimp_mesh, std::string file_dir);
        static void ProcessAssimpMaterial(Scene *scene, const S<SceneNode>& node, const aiMaterial* assimp_material, std::string file_dir);
    };
#endif
}

#endif