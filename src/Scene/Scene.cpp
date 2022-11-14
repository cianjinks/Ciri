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
}