#ifndef CIRI_SCENE_H
#define CIRI_SCENE_H

#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Material.h"

#include "entt.hpp"
#include <glm/glm.hpp>

namespace Ciri
{
    enum class Importer
    {
        OBJ = 0,
        GLTF,
        ASSIMP
    };

    class Entity;

    /* TODO: If a scene object is created not as a shared ptr then problems will result from `SharedThis`. */
    /* Option might be to make default constructor private and implement a `Create()` function. */
    class Scene : public SharedThis<Scene>
    {
    public:
        const char *Name;

    private:
        entt::registry m_Registry;
        MaterialLibrary m_MaterialLibrary;
        // TODO: MeshLibrary/Manager?

    public:
        Scene(const char *name);
        ~Scene() = default;

        entt::registry &GetRegistry() { return m_Registry; }
        MaterialLibrary &GetMaterialLibrary() { return m_MaterialLibrary; }

        Entity CreateEntity(const std::string &tag);
        S<Material> CreateMaterial(MaterialSpecification &spec);

        friend Entity;
    };
}

#endif