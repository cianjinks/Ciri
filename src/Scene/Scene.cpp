#include "Scene.h"

#include "Import/OBJImporter.h"
#include "Import/GLTFImporter.h"
#include "Import/AssimpImporter.h"

namespace Ciri
{
	void SceneNode::AddChild(S<SceneNode> child)
	{
		Children.push_back(child);
	}

	Scene::Scene(const char *name)
		: Name(name)
	{
		m_Root = CreateS<SceneNode>();
		m_Root->Name = std::string(name);

		// Default Material
		MaterialInfo info = {"default", false};
		m_DefaultMaterial = MatLib.CreateMaterial(info, glm::vec3(1.0f, 0.0f, 1.0f));
	}

	Scene::~Scene()
	{
		// Destroy scene node tree
		DestroyTree(m_Root);
	}

	void Scene::DestroyTree(S<SceneNode> root)
	{
		for (S<SceneNode> node : root->Children)
		{
			DestroyTree(node);
		}
		root->Children.clear(); // Destroys all elements in vector?
	}

	S<SceneNode> Scene::CreateMesh(const char *name, S<Mesh> mesh, S<Material> material)
	{
		S<SceneNode> node = CreateS<SceneNode>();

		node->Name = name;
		node->NodeMesh = mesh;
		node->NodeMaterial = material ? material : m_DefaultMaterial;

		m_TotalTriCount += mesh->TriCount;
		m_MeshCount++;

		return node;
	}

	S<SceneNode> Scene::AddMesh(const char *name, S<Mesh> mesh, S<Material> material)
	{
		S<SceneNode> node = CreateS<SceneNode>();

		node->Name = name;
		node->NodeMesh = mesh;
		node->NodeMaterial = material ? material : m_DefaultMaterial;
		m_Root->AddChild(node);

		m_TotalTriCount += mesh->TriCount;
		m_MeshCount++;

		return node;
	}

	S<SceneNode> Scene::AddContainer()
	{
		S<SceneNode> node = CreateS<SceneNode>();

		m_Root->AddChild(node);

		return node;
	}

	S<SceneNode> Scene::AddContainer(const char *name)
	{
		S<SceneNode> node = CreateS<SceneNode>();

		node->Name = name;
		m_Root->AddChild(node);

		return node;
	}

	S<SceneNode> Scene::LoadModel(Importer type, const char *name, const char *filepath)
	{
		S<SceneNode> result = nullptr;
		switch (type)
		{
		case Importer::OBJ:
		{
			result = OBJImporter::Import(this, filepath);
			break;
		}
		case Importer::GLTF:
		{
			result = GLTFImporter::Import(this, filepath);
			break;
		}
		case Importer::ASSIMP:
			result = AssimpImporter::Import(this, filepath);
			break;
		}

		if (result)
		{
			result->Name = name;
		}

		return result;
	}
}