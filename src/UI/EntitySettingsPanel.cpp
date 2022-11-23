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
                ImGui::Separator();
            }

            if (m_SelectedEntity.HasComponent<MaterialComponent>())
            {
                ImGui::Text("Material");
                auto &mc = m_SelectedEntity.GetComponent<MaterialComponent>();
                if (ImGui::BeginCombo("##", mc.CMaterial->spec.name.c_str(), 0))
                {
                    /* TODO: Access material library to allow choosing material. */
                    ImGui::EndCombo();
                }
                ImGui::Separator();
            }

            if (m_SelectedEntity.HasComponent<LightComponent>())
            {
                ImGui::Text("Light");
                auto &lc = m_SelectedEntity.GetComponent<LightComponent>();
                ImGui::ColorEdit3("Ambient", &lc.Ambient.x);
                ImGui::ColorEdit3("Diffuse", &lc.Diffuse.x);
                ImGui::ColorEdit3("Specular", &lc.Specular.x);
                ImGui::Separator();
            }
        }
        ImGui::End();
    }
}