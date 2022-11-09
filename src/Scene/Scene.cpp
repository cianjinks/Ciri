#include "Scene.h"
#include "Entity.h"

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
		return entity;
	}
}