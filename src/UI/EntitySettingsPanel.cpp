#include "EntitySettingsPanel.h"

#include "imgui.h"

#if 0
namespace Ciri
{
    EntitySettingsPanel::EntitySettingsPanel(MaterialLibrary &matlib)
        : m_MaterialLibrary(matlib)
    {
    }

    void EntitySettingsPanel::OnUIRender()
    {
        ImGui::Begin("Mesh Settings");
        if (m_SelectedNode)
        {
            ImGui::Text(m_SelectedNode->Name.c_str());
            ImGui::InputFloat3("Position", &m_SelectedNode->Position.x);
            glm::vec3 rotation = glm::degrees(m_SelectedNode->Rotation);
            ImGui::InputFloat3("Rotation", &rotation.x);
            m_SelectedNode->Rotation = glm::radians(rotation);
            ImGui::InputFloat3("Scale", &m_SelectedNode->Scale.x);

            // Material setting
            if (m_SelectedNode->NodeMesh)
            {
                if (ImGui::BeginCombo("Material", m_SelectedNode->NodeMaterial->info.name.c_str(), 0))
                {
                    for (auto &pair : m_MaterialLibrary.GetMaterials())
                    {
                        S<Material> material = pair.second;
                        const bool is_selected = (m_SelectedNode->NodeMaterial == material);
                        if (ImGui::Selectable(material->info.name.c_str(), is_selected))
                            m_SelectedNode->NodeMaterial = material;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
        }
        ImGui::End();
    }
}
#endif