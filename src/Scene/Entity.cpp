#include "Entity.h"

#include "Component.h"

namespace Ciri
{
    Entity::Entity(entt::entity entity, const S<Scene> &scene)
        : m_Entity(entity), m_Scene(scene)
    {
    }

    void Entity::SetParent(Entity parent)
    {
        HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();

        /* Remove from old parent's children */
        if (hierarchy.Parent.IsValid())
        {
            HierarchyComponent &parent_hierarchy = hierarchy.Parent.GetComponent<HierarchyComponent>();

            Entity child = parent_hierarchy.First;
            while (child.IsValid())
            {
                HierarchyComponent &child_hierarchy = child.GetComponent<HierarchyComponent>();

                if (child == *this)
                {
                    if (child_hierarchy.Prev.IsValid())
                    {
                        child_hierarchy.Prev = child_hierarchy.Next;
                    }
                    else
                    {
                        parent_hierarchy.First = child_hierarchy.Next;
                    }
                }

                child = child_hierarchy.Next;
            }
        }

        hierarchy.Parent = parent;

        /* Add to new parent's children. */
        HierarchyComponent &parent_hierarchy = hierarchy.Parent.GetComponent<HierarchyComponent>();
        if (parent_hierarchy.First.IsValid())
        {
            Entity child = parent_hierarchy.First;
            HierarchyComponent &child_hierarchy = child.GetComponent<HierarchyComponent>();
            while (child_hierarchy.Next.IsValid())
            {
                child_hierarchy = child.GetComponent<HierarchyComponent>();
            }
            child_hierarchy.Next = *this;
        }
        else
        {
            parent_hierarchy.First = *this;
        }
    }
}