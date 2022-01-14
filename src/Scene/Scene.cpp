#include "Scene.h"

namespace Ciri
{
    void SceneNode::AddChild(SceneNode *child)
    {
        m_Children.push_back(child);
    }

    Scene::Scene(const char *name)
        : Name(name)
    {
        m_Root = new SceneNode();
        m_Root->Name = name;
    }

    Scene::~Scene()
    {
        // Destroy scene node tree
        DestroyTree(m_Root);
        delete m_Root;
    }

    void Scene::DestroyTree(SceneNode *root)
    {
        for (SceneNode *node : root->m_Children)
        {
            DestroyTree(node);
        }
        root->m_Children.clear(); // Destroys all elements in vector?
    }

    SceneNode *Scene::AddMesh(const char *name, Mesh *mesh)
    {
        SceneNode *node = new SceneNode();

        // Check for duplicates
        if (m_NameLookup.find(name) != m_NameLookup.end())
        {
            std::cout << "Failed to add mesh to scene with duplicate name: " << name << std::endl;
            return node;
        }
        m_NameLookup.insert(name);

        node->Name = name;
        node->NodeMesh = mesh;
        m_Root->AddChild(node);

        m_TotalTriCount += mesh->TriCount;
        m_MeshCount++;

        return node;
    }
}