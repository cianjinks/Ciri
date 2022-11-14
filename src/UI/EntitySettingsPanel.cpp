#include "EntitySettingsPanel.h"

#include "imgui.h"

namespace Ciri
{
    void EntitySettingsPanel::OnUIRender()
    {
        ImGui::Begin("Entity Settings");
        if (m_SelectedEntity.IsValid())
        {
            if (m_SelectedEntity.HasComponent<TagComponent>())
            {
                auto &tc = m_SelectedEntity.GetComponent<TagComponent>();
                ImGui::Text(tc.Tag.c_str());
            }
            else
            {
                ImGui::Text("Unknown Entity?");
            }
            ImGui::Separator();

            if (m_SelectedEntity.HasComponent<TransformComponent>())
            {
                auto &tc = m_SelectedEntity.GetComponent<TransformComponent>();
                glm::vec3 translation = tc.Transform.GetLocalTranslation();
                glm::vec3 rotation = glm::degrees(glm::eulerAngles(tc.Transform.GetLocalRotation()));
                glm::vec3 scale = tc.Transform.GetLocalScale();
                ImGui::InputFloat3("Translation", &translation.x);
                ImGui::InputFloat3("Rotation", &rotation.x);
                ImGui::InputFloat3("Scale", &scale.x);
                tc.Transform.SetLocalTranslation(translation);
                tc.Transform.SetLocalRotation(glm::radians(rotation));
                tc.Transform.SetLocalScale(scale);
            }

            // // Material setting
            // if (m_SelectedNode->NodeMesh)
            // {
            //     if (ImGui::BeginCombo("Material", m_SelectedNode->NodeMaterial->info.name.c_str(), 0))
            //     {
            //         for (auto &pair : m_MaterialLibrary.GetMaterials())
            //         {
            //             S<Material> material = pair.second;
            //             const bool is_selected = (m_SelectedNode->NodeMaterial == material);
            //             if (ImGui::Selectable(material->info.name.c_str(), is_selected))
            //                 m_SelectedNode->NodeMaterial = material;
            //             if (is_selected)
            //                 ImGui::SetItemDefaultFocus();
            //         }
            //         ImGui::EndCombo();
            //     }
            // }
        }
        ImGui::End();
    }
}