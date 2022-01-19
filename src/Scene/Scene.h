#ifndef CIRI_SCENE_H
#define CIRI_SCENE_H

#include <vector>
#include <unordered_set>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include "Mesh/Mesh.h"
#include "Render/Material.h"

namespace Ciri
{
    class SceneNode
    {
    public:
        std::string Name = "";

        Mesh *NodeMesh = nullptr;
        Material *NodeMaterial = nullptr; // Reference to a material in the Scene's MaterialLibrary

        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);

        // Public for basic rendering tests
        std::vector<SceneNode *> m_Children;

    public:
        void AddChild(SceneNode *child);
    };

    class Scene
    {
    public:
        const char *Name;
        MaterialLibrary MatLib;

    private:
        SceneNode *m_Root = nullptr;
        uint32_t m_MeshCount = 0;
        uint32_t m_TotalTriCount = 0;

    public:
        Scene(const char *name);
        ~Scene();

    public:
        SceneNode *AddMesh(const char *name, Mesh *mesh);
        SceneNode *AddContainer(const char *name);                    // Create empty scene node
        SceneNode *LoadModel(const char *name, const char *filepath); // Load .obj file

        SceneNode *GetRoot() const
        {
            return m_Root;
        }
        const uint32_t GetMeshCount() const { return m_MeshCount; }
        const uint32_t GetTotalTriCount() const { return m_TotalTriCount; }

    private:
        void DestroyTree(SceneNode *root);
    };
}

#endif