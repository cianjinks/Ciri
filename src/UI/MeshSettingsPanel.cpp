#include "MeshSettingsPanel.h"

#include "imgui.h"

namespace Ciri
{
    MeshSettingsPanel::MeshSettingsPanel(MaterialLibrary &matlib)
        : m_MaterialLibrary(matlib)
    {
    }

    void MeshSettingsPanel::OnUIRender()
    {
        ImGui::Begin("Mesh Settings");
        if (m_SelectedNode)
        {
            ImGui::Text(m_SelectedNode->Name.c_str());
            ImGui::InputFloat3("Position", &m_SelectedNode->Position.x);
            ImGui::InputFloat3("Rotation", &m_SelectedNode->Rotation.x);
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