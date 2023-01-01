#include "MaterialSettingsPanel.h"

#include "imgui.h"

namespace Ciri
{
    MaterialSettingsPanel::MaterialSettingsPanel(MaterialLibrary &matlib)
        : m_MaterialLibrary(matlib)
    {
    }

    void MaterialSettingsPanel::OnUIRender()
    {
        ImGui::Begin("Material Settings");
        if (m_SelectedMaterial)
        {
            ImGui::Text(m_SelectedMaterial->spec.name.c_str());
            ImGui::ColorEdit3("Base Color", &m_SelectedMaterial->spec.baseColor.x);
            CreateTextureCombo("Albedo Tex", m_SelectedMaterial->spec.baseColorFilepath, m_SelectedMaterial->spec.baseColorTextureID);
            CreateTextureCombo("Normal Tex", m_SelectedMaterial->spec.normalFilepath, m_SelectedMaterial->spec.normalTextureID);
            CreateTextureCombo("Metallic Tex", m_SelectedMaterial->spec.metallicRoughnessFilepath, m_SelectedMaterial->spec.metallicRoughnessTextureID);
            CreateTextureCombo("Emissive Tex", m_SelectedMaterial->spec.emissiveFilepath, m_SelectedMaterial->spec.emissiveTextureID);
            CreateTextureCombo("Occlusion Tex", m_SelectedMaterial->spec.occlusionFilepath, m_SelectedMaterial->spec.occlusionTextureID);
            ImGui::InputFloat("Subsurface", &m_SelectedMaterial->spec.subsurface);
            ImGui::InputFloat("Metallic", &m_SelectedMaterial->spec.metallic);
            ImGui::InputFloat("Specular", &m_SelectedMaterial->spec.specular);
            ImGui::InputFloat("Specular Tint", &m_SelectedMaterial->spec.specularTint);
            ImGui::InputFloat("Roughness", &m_SelectedMaterial->spec.roughness);
            ImGui::InputFloat("Anisotropic", &m_SelectedMaterial->spec.anisotropic);
            ImGui::InputFloat("Sheen", &m_SelectedMaterial->spec.sheen);
            ImGui::InputFloat("Sheen Tint", &m_SelectedMaterial->spec.sheenTint);
            ImGui::InputFloat("Clearcoat", &m_SelectedMaterial->spec.clearcoat);
            ImGui::InputFloat("Clearcoat Gloss", &m_SelectedMaterial->spec.clearcoatGloss);
        }
        ImGui::End();
    }

    void MaterialSettingsPanel::CreateTextureCombo(std::string name, std::string &material_tex_name, uint32_t &material_tex_id)
    {
        std::string texture_name = "None";
        if (!material_tex_name.empty())
        {
            texture_name = material_tex_name;
        }

        if (ImGui::BeginCombo(name.c_str(), texture_name.c_str(), 0))
        {
            for (auto &pair : m_MaterialLibrary.GetTextures())
            {
                std::string tex_name = pair.first;
                uint32_t texture_id = pair.second;
                const bool is_selected = (material_tex_id == texture_id);
                if (ImGui::Selectable(tex_name.c_str(), is_selected))
                {
                    material_tex_id = texture_id;
                    material_tex_name = tex_name;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}