#include "Scene.h"

#include "Entity.h"
#include "Component.h"

#include "Import/OBJImporter.h"
#include "Import/GLTFImporter.h"
#include "Import/AssimpImporter.h"

namespace Ciri
{
	Scene::Scene(const char *name)
		: Name(name)
	{
	}

	Entity Scene::CreateEntity(const std::string &tag)
	{
		Entity entity(m_Registry.create(), shared_from_this());
		entity.AddComponent<TagComponent>(tag);
		entity.AddComponent<HierarchyComponent>();
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	S<Material> Scene::CreateMaterial(MaterialSpecification &spec)
	{
		return m_MaterialLibrary.CreateMaterial(spec);
	}

	S<Material> Scene::GetMaterial(std::string name)
	{
		return m_MaterialLibrary.GetMaterial(name);
	}

	Entity Scene::LoadModel(Importer type, const std::string &tag, const std::string &path)
	{
		Entity entity = CreateEntity(tag);
		switch (type)
		{
		case Importer::OBJ:
		{
			CIRI_ASSERT(false, ".obj importer is not ported to ECS yet!");
			break;
		}
		case Importer::GLTF:
		{
			CIRI_ASSERT(false, ".gltf importer is not ported to ECS yet!");
			break;
		}
		case Importer::ASSIMP:
		{
			AssimpImporter::Import(shared_from_this(), entity, path);
			break;
		}
		}
		return entity;
	}
}