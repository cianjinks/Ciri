#ifndef CIRI_SCENE_H
#define CIRI_SCENE_H

#include <glm/glm.hpp>

#include "Mesh/Mesh.h"
#include "Render/Material.h"

namespace Ciri
{
    enum class ModelType
    {
        OBJ = 0,
        GLTF
    };

    class SceneNode
    {
    public:
        std::string Name = "";

        S<Mesh> NodeMesh = nullptr;
        S<Material> NodeMaterial = nullptr; // Reference to a material in the Scene's MaterialLibrary

        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);

        std::vector<S<SceneNode>> Children;

    public:
        void AddChild(S<SceneNode> child);
    };

    class Scene
    {
    public:
        const char *Name;
        MaterialLibrary MatLib;

    private:
        S<SceneNode> m_Root = nullptr;
        uint32_t m_MeshCount = 0;
        uint32_t m_TotalTriCount = 0;

        S<Material> m_DefaultMaterial;

    public:
        Scene(const char *name);
        ~Scene();

    public:
        S<SceneNode> AddMesh(const char *name, S<Mesh> mesh, S<Material> material = nullptr);
        S<SceneNode> AddContainer();                 // Create empty scene node
        S<SceneNode> AddContainer(const char *name); // Create empty scene node

        S<SceneNode> LoadModel(ModelType type, const char *name, const char *filepath);

        S<SceneNode> GetRoot() const { return m_Root; }
        const uint32_t GetMeshCount() const { return m_MeshCount; }
        const uint32_t GetTotalTriCount() const { return m_TotalTriCount; }
        S<Material> GetDefaultMaterial() const { return m_DefaultMaterial; }

        // TODO: These shouldn't be necesary, instead store count on nodes in the tree?
        void SetMeshCount(uint32_t meshcount) { m_MeshCount = meshcount; }
        void SetTotalTriCount(uint32_t tricount) { m_TotalTriCount = tricount; }

    private:
        void DestroyTree(S<SceneNode> root);
    };
}

#endif