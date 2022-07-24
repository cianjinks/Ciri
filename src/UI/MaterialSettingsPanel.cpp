#include "MaterialSettingsPanel.h"

#include "imgui.h"

namespace Ciri
{
    MaterialSettingsPanel::MaterialSettingsPanel(MaterialLibrary& matlib)
        : m_MaterialLibrary(matlib)
    {
    }

    void MaterialSettingsPanel::OnUIRender()
    {
        ImGui::Begin("Material Settings");
        if (m_SelectedMaterial)
        {
            ImGui::Text(m_SelectedMaterial->info.name.c_str());
            ImGui::InputFloat3("Base Color", &m_SelectedMaterial->baseColor.x);
            CreateTextureCombo("Albedo Tex", m_SelectedMaterial->info.baseColorFilepath, m_SelectedMaterial->baseColorTextureID);
            CreateTextureCombo("Normal Tex", m_SelectedMaterial->info.normalFilepath, m_SelectedMaterial->normalTextureID);
            CreateTextureCombo("Metallic Tex", m_SelectedMaterial->info.metallicRoughnessFilepath, m_SelectedMaterial->metallicRoughnessTextureID);
            CreateTextureCombo("Emissive Tex", m_SelectedMaterial->info.emissiveFilepath, m_SelectedMaterial->emissiveTextureID);
            CreateTextureCombo("Occlusion Tex", m_SelectedMaterial->info.occlusionFilepath, m_SelectedMaterial->occlusionTextureID);
            ImGui::InputFloat("Subsurface", &m_SelectedMaterial->subsurface);
            ImGui::InputFloat("Metallic", &m_SelectedMaterial->metallic);
            ImGui::InputFloat("Specular", &m_SelectedMaterial->specular);
            ImGui::InputFloat("Specular Tint", &m_SelectedMaterial->specularTint);
            ImGui::InputFloat("Roughness", &m_SelectedMaterial->roughness);
            ImGui::InputFloat("Anisotropic", &m_SelectedMaterial->anisotropic);
            ImGui::InputFloat("Sheen", &m_SelectedMaterial->sheen);
            ImGui::InputFloat("Sheen Tint", &m_SelectedMaterial->sheenTint);
            ImGui::InputFloat("Clearcoat", &m_SelectedMaterial->clearcoat);
            ImGui::InputFloat("Clearcoat Gloss", &m_SelectedMaterial->clearcoatGloss);
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