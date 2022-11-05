#ifndef CIRI_ASSIMP_H
#define CIRI_ASSIMP_H

#include "Scene/Scene.h"
#include "Anim/Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Ciri
{
    class AssimpImporter
    {
    public:
        static S<SceneNode> Import(Scene* scene, const char *filepath);

    private:
        static void ProcessAssimpMesh(Scene* scene, const S<SceneNode>& container, const aiScene* assimp_scene, const aiMesh* assimp_mesh, std::string file_dir, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter);
        static void ProcessAssimpMaterial(Scene *scene, const S<SceneNode>& node, const aiMaterial* assimp_material, std::string file_dir);
        static void ProcessBones(const aiScene* assimp_scene, const aiMesh* assimp_mesh, std::vector<glm::i32vec4>& boneids, std::vector<glm::vec4>& boneweights, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter);
        static void ProcessAnimation(const aiAnimation* assimp_anim, const S<SceneNode>& container, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter);

        static void SetNodeTransform(S<SceneNode> parent, S<SceneNode> node, aiNode* assimp_node);
    };
}

#endif