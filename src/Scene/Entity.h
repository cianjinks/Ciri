#ifndef CIRI_ENTITY_H
#define CIRI_ENTITY_H

#include "Scene.h"
#include "Component.h"

#include "entt.hpp"

namespace Ciri
{
    class Entity
    {
    private:
        entt::entity m_Entity = entt::null;
        W<Scene> m_Scene;

    public:
        Entity() = default;
        ~Entity() = default;
        Entity(const Entity &other) = default;
        Entity(entt::entity entity, const S<Scene> &scene);

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            CIRI_ASSERT(!HasComponent<T>(), "Entity already has component!");
            if (S<Scene> scene = m_Scene.lock())
            {
                return scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
            }
            else
            {
                CIRI_ASSERT(false, "Entity tried to use a dangling scene reference!");
            }
        }

        template <typename T>
        T &GetComponent()
        {
            CIRI_ASSERT(HasComponent<T>(), "Entity is missing component!");
            if (S<Scene> scene = m_Scene.lock())
            {
                return scene->m_Registry.get<T>(m_Entity);
            }
            else
            {
                CIRI_ASSERT(false, "Entity tried to use a dangling scene reference!");
            }
        }

        template <typename T>
        bool HasComponent()
        {
            if (S<Scene> scene = m_Scene.lock())
            {
                return scene->m_Registry.all_of<T>(m_Entity);
            }
            else
            {
                CIRI_ASSERT(false, "Entity tried to use a dangling scene reference!");
            }
        }
    };
}

#endif