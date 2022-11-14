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

        UpdateTransforms();
    }

    /* Ensure the transform heirarchy for this entity is correct.
       It's a bit strange to do this here, probably better implemented in the Scene or some kind of `SceneGraph`.  */
    void Entity::UpdateTransforms()
    {
        Entity current = *this;
        HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();

        glm::mat4 parent_matrix = glm::mat4(1.0f);
        if (hierarchy.Parent.IsValid())
        {
            TransformComponent &tcp = hierarchy.Parent.GetComponent<TransformComponent>();
            parent_matrix = tcp.Transform.GetWorldMatrix();
        }

        /* Propogate transforms throughout the subtree. */
        std::stack<std::pair<Entity, glm::mat4>> stack;
        stack.push({current, parent_matrix});
        while (!stack.empty())
        {
            auto [entity, parent_transform] = stack.top();
            stack.pop();

            TransformComponent &tc = entity.GetComponent<TransformComponent>();
            tc.Transform.SetParentMatrix(parent_transform);
            parent_transform = tc.Transform.GetWorldMatrix();

            /* Add children to stack. */
            HierarchyComponent entity_hierarchy = entity.GetComponent<HierarchyComponent>();
            if (entity_hierarchy.First.IsValid())
            {
                Entity child = entity_hierarchy.First;
                while (child.IsValid())
                {
                    stack.push({child, parent_transform});
                    entity_hierarchy = child.GetComponent<HierarchyComponent>();
                    child = entity_hierarchy.Next;
                }
            }
        }
    }
}