#include "SceneHierarchyPanel.h"

#include "imgui.h"
#include "IconsForkAwesome.h"

namespace Ciri
{
    SceneHierarchyPanel::SceneHierarchyPanel(const S<Scene>& scene)
    {
        SetScene(scene);
    }

    void SceneHierarchyPanel::OnUIRender()
    {
        ImGui::Begin("Scene");
        RenderNodeHierarchy(m_Scene->GetRoot(), 0);
        ImGui::End();
    }

    void SceneHierarchyPanel::RenderNodeHierarchy(SceneNode *root, int ptr_id)
    {   
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

        bool is_leaf = root->Children.empty();
        if (is_leaf)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        if (m_SelectedNode == root)
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
        // TODO: Blank root name is buggy and icon handling is rough
        bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)ptr_id, node_flags, "%s %s", root->NodeMesh ? ICON_FK_CUBE : ICON_FK_SQUARE_O, root->Name.c_str());

        // Update selected node on click but not expand toggle click
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            m_SelectedNode = root;
        }

        if (node_open && !is_leaf)
        {
            for (SceneNode *node : root->Children)
            {
                RenderNodeHierarchy(node, ptr_id++);
            }

            ImGui::TreePop();
        }
        ImGui::PopStyleVar();
    }
}