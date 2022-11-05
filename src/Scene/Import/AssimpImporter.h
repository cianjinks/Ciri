#ifndef CIRI_ASSIMP_H
#define CIRI_ASSIMP_H

#include "Scene/Scene.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtx/quaternion.hpp>

namespace Ciri
{
    class AssimpImporter
    {
    private:
        struct BoneInfo
        {
            int id;
            glm::mat4 offset;
        };

    public:
        static S<SceneNode> Import(Scene* scene, const char *filepath);

    private:
        static void ProcessAssimpMesh(Scene* scene, const S<SceneNode>& container, const aiScene* assimp_scene, const aiMesh* assimp_mesh, std::string file_dir, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter);
        static void ProcessAssimpMaterial(Scene *scene, const S<SceneNode>& node, const aiMaterial* assimp_material, std::string file_dir);
        static void ProcessBones(const aiScene* assimp_scene, const aiMesh* assimp_mesh, std::vector<glm::i32vec4>& boneids, std::vector<glm::vec4>& boneweights, std::map<std::string, BoneInfo>& boneinfomap, int& bonecounter);

        static void SetNodeTransform(S<SceneNode> parent, S<SceneNode> node, aiNode* assimp_node);

        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
            glm::mat4 to;
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
            return to;
        }
    };
}

#endif