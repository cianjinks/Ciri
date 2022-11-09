#include "Entity.h"

namespace Ciri
{
    Entity::Entity(entt::entity entity, const S<Scene> &scene)
        : m_Entity(entity), m_Scene(scene)
    {
    }
}