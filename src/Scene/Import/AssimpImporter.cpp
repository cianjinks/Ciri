#include "AssimpImporter.h"

namespace Ciri
{
    S<SceneNode> AssimpImporter::Import(Scene *scene, const char *filepath)
    {
        S<SceneNode> container = scene->AddContainer();

        Assimp::Importer importer;
        const aiScene *assimp_scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_FindInvalidData);

        if (!assimp_scene || assimp_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE | !assimp_scene->mRootNode)
        {
            CIRI_ERROR("{}", filepath);
            CIRI_ASSERT(false, "Failed to load model using assimp!");
        }

        std::string file_dir = std::string(filepath);
        file_dir = file_dir.substr(0, file_dir.find_last_of('/'));

        std::map<std::string, BoneInfo> boneInfoMap;
        int boneCounter = 0;

        struct NodeStackElem
        {
            S<SceneNode> parent;
            aiNode *node;
        };
        std::stack<NodeStackElem> nodestack;
        nodestack.push({container, assimp_scene->mRootNode});
        while (!nodestack.empty())
        {
            NodeStackElem elem = nodestack.top();
            nodestack.pop();

            S<SceneNode> node = CreateS<SceneNode>();
            node->Name = std::string(elem.node->mName.C_Str());
            SetNodeTransform(elem.parent, node, elem.node);
            for (uint32_t m = 0; m < elem.node->mNumMeshes; m++)
            {
                aiMesh *assimp_mesh = assimp_scene->mMeshes[elem.node->mMeshes[m]];
                ProcessAssimpMesh(scene, node, assimp_scene, assimp_mesh, file_dir, boneInfoMap, boneCounter);
            }
            elem.parent->AddChild(node);

            for (uint32_t n = 0; n < elem.node->mNumChildren; n++)
            {
                nodestack.push({node, elem.node->mChildren[n]});
            }
        }

        return container;
    }

    void AssimpImporter::ProcessAssimpMesh(Scene *scene, const S<SceneNode> &node, const aiScene *assimp_scene, const aiMesh *assimp_mesh, std::string file_dir, std::map<std::string, BoneInfo> &boneinfomap, int &bonecounter)
    {
        std::vector<glm::vec3> positionData;
        std::vector<glm::vec3> normalData;
        std::vector<glm::vec2> texCoordData;

        for (uint32_t v = 0; v < assimp_mesh->mNumVertices; v++)
        {
            positionData.push_back({assimp_mesh->mVertices[v].x, assimp_mesh->mVertices[v].y, assimp_mesh->mVertices[v].z});
            normalData.push_back({assimp_mesh->mNormals[v].x, assimp_mesh->mNormals[v].y, assimp_mesh->mNormals[v].z});
            if (assimp_mesh->mTextureCoords[0])
            {
                texCoordData.push_back({assimp_mesh->mTextureCoords[0][v].x, assimp_mesh->mTextureCoords[0][v].y});
            }
            else
            {
                texCoordData.push_back({0.0f, 0.0f});
            }
        }

        std::vector<uint16_t> indexData;
        for (uint32_t i = 0; i < assimp_mesh->mNumFaces; i++)
        {
            aiFace face = assimp_mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++)
            {
                indexData.push_back((uint16_t)face.mIndices[j]);
            }
        }

        std::vector<glm::i32vec4> boneidData;
        std::vector<glm::vec4> boneweightData;
        ProcessBones(assimp_scene, assimp_mesh, boneidData, boneweightData, boneinfomap, bonecounter);

        S<Mesh>
            mesh = CreateS<Mesh>(positionData, normalData, texCoordData, indexData, boneidData, boneweightData);
        mesh->Construct();

        node->NodeMesh = mesh;
        if (assimp_mesh->mMaterialIndex >= 0)
        {
            aiMaterial *assimp_material = assimp_scene->mMaterials[assimp_mesh->mMaterialIndex];
            ProcessAssimpMaterial(scene, node, assimp_material, file_dir);

            if (!node->NodeMaterial)
            {
                node->NodeMaterial = scene->GetDefaultMaterial();
            }
        }

        if (assimp_scene->mAnimations[0])
        {
            ProcessAnimation(assimp_scene, assimp_scene->mAnimations[0], node, boneinfomap, bonecounter);
        }
    }

    void AssimpImporter::ProcessAssimpMaterial(Scene *scene, const S<SceneNode> &node, const aiMaterial *assimp_material, std::string file_dir)
    {
        MaterialInfo info = {"", true, false};
        bool valid_mat = false;

        /* Only support one diffuse texture for now. */
        uint32_t diffuse_texture_count = assimp_material->GetTextureCount(aiTextureType_DIFFUSE);
        if (diffuse_texture_count > 0)
        {
            aiString local_filepath;
            assimp_material->GetTexture(aiTextureType_DIFFUSE, 0, &local_filepath);
            std::string full_filepath = file_dir + "/" + std::string(local_filepath.C_Str());

            info.name = std::string(local_filepath.C_Str());
            info.baseColorFilepath = full_filepath;
            valid_mat = true;
        }

        if (diffuse_texture_count > 1)
        {
            CIRI_WARN("Model has more than 2 diffuse textures for one mesh. This is unsupported.");
        }


        /* Base color support. */
        aiColor3D color(0.f, 0.f, 0.f);
        glm::vec3 baseColor = glm::vec3(1.0f);
        if(assimp_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        {
            baseColor = {color.r, color.g, color.b};
            valid_mat = true;
        }

        if (valid_mat)
        {
            S<Material> material = scene->MatLib.CreateMaterial(info, baseColor);
            node->NodeMaterial = material;
        }
    }

    void AssimpImporter::ProcessBones(const aiScene *assimp_scene, const aiMesh *assimp_mesh, std::vector<glm::i32vec4> &boneids, std::vector<glm::vec4> &boneweights, std::map<std::string, BoneInfo> &boneinfomap, int &bonecounter)
    {
        for (int bone_index = 0; bone_index < assimp_mesh->mNumBones; bone_index++)
        {
            int boneID = -1;
            std::string boneName = assimp_mesh->mBones[bone_index]->mName.C_Str();
            if (boneinfomap.find(boneName) == boneinfomap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = bonecounter;
                newBoneInfo.offset = Math::AssimpConvertMatrixToGLMFormat(assimp_mesh->mBones[bone_index]->mOffsetMatrix);
                boneinfomap[boneName] = newBoneInfo;
                boneID = bonecounter;
                bonecounter++;
            }
            else
            {
                boneID = boneinfomap[boneName].id;
            }

            auto weights = assimp_mesh->mBones[bone_index]->mWeights;
            uint32_t numWeights = assimp_mesh->mBones[bone_index]->mNumWeights;
            for (int weight_index = 0; weight_index < numWeights; ++weight_index)
            {
                int vertex_id = weights[weight_index].mVertexId;
                float weight = weights[weight_index].mWeight;

                for (int i = 0; i < 4; ++i)
                {
                    if (boneids.size() <= vertex_id)
                    {
                        boneids.resize(vertex_id + 1, glm::i32vec4(-1));
                    }
                    if (boneweights.size() <= vertex_id)
                    {
                        boneweights.resize(vertex_id + 1, glm::vec4(0.0f));
                    }
                    if (boneids[vertex_id][i] < 0)
                    {
                        boneweights[vertex_id][i] = weight;
                        boneids[vertex_id][i] = boneID;
                        break;
                    }
                }
            }
        }
    }

    void AssimpImporter::ProcessAnimation(const aiScene *scene, const aiAnimation *assimp_anim, const S<SceneNode> &container, std::map<std::string, BoneInfo> &boneinfomap, int &bonecounter)
    {
        container->NodeAnimation = CreateS<Animation>(scene, assimp_anim, boneinfomap, bonecounter);
    }

    void AssimpImporter::SetNodeTransform(S<SceneNode> parent, S<SceneNode> node, aiNode *assimp_node)
    {
        glm::mat4 node_transform = Math::AssimpConvertMatrixToGLMFormat(assimp_node->mTransformation);
        Math::DecomposeTransform(node_transform, node->Position, node->Rotation, node->Scale);
    }
}