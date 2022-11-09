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
        int e = 0;
        registry.each([&](auto entity)
                      {
            if (registry.valid(entity))
            {
                Entity entity(entity, m_Scene);
                RenderNode(entity);
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

        auto &tc = entity.GetComponent<TagComponent>();
        bool node_open = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, node_flags, "%s %s", ICON_FK_CUBE, tc.Tag.c_str());

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            m_SelectedEntity = entity;
        }

        if (node_open)
        {
            ImGui::TreePop();
        }
    }

    // void SceneHierarchyPanel::RenderNodeHierarchy(S<SceneNode> root, int ptr_id)
    // {
    //     ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

    //     bool is_leaf = root->Children.empty();
    //     if (is_leaf)
    //     {
    //         node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    //     }

    //     if (m_SelectedNode == root)
    //     {
    //         node_flags |= ImGuiTreeNodeFlags_Selected;
    //     }

    //     ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
    //     // TODO: Blank root name is buggy and icon handling is rough
    //     bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)ptr_id, node_flags, "%s %s", root->NodeMesh ? ICON_FK_CUBE : ICON_FK_SQUARE_O, root->Name.c_str());

    //     // Update selected node on click but not expand toggle click
    //     if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    //     {
    //         m_SelectedNode = root;
    //     }

    //     if (node_open && !is_leaf)
    //     {
    //         for (S<SceneNode> node : root->Children)
    //         {
    //             RenderNodeHierarchy(node, ptr_id++);
    //         }

    //         ImGui::TreePop();
    //     }
    //     ImGui::PopStyleVar();
    // }
}