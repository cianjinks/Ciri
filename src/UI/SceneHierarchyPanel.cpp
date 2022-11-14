#include "SceneHierarchyPanel.h"

#include "imgui.h"
#include "IconsForkAwesome.h"

namespace Ciri
{
    SceneHierarchyPanel::SceneHierarchyPanel(const S<Scene> &scene)
    {
        SetScene(scene);
    }

    void SceneHierarchyPanel::OnUIRender()
    {
        ImGui::Begin("Scene");
        const entt::registry &registry = m_Scene->GetRegistry();
        registry.each([&](auto entity)
                      {
            if (registry.valid(entity))
            {
                Entity entity(entity, m_Scene);
                HierarchyComponent &hierarchy = entity.GetComponent<HierarchyComponent>();
                if (!hierarchy.Parent.IsValid())
                {
                    RenderNode(entity);
                }
            } });
        ImGui::End();
    }

    void SceneHierarchyPanel::RenderNode(Entity entity)
    {
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
        if (m_SelectedEntity.IsValid() && m_SelectedEntity == entity)
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        HierarchyComponent &hierarchy = entity.GetComponent<HierarchyComponent>();
        bool is_leaf = !hierarchy.First.IsValid();
        if (is_leaf)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        auto &tc = entity.GetComponent<TagComponent>();
        bool node_open = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, node_flags, "%s %s", ICON_FK_CUBE, tc.Tag.c_str());

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            m_SelectedEntity = entity;
        }

        if (node_open && !is_leaf)
        {
            Entity child = hierarchy.First;
            while (child.IsValid())
            {
                RenderNode(child);

                HierarchyComponent &child_hierarchy = child.GetComponent<HierarchyComponent>();
                child = child_hierarchy.Next;
            }
            ImGui::TreePop();
        }
    }
}