#include "RendererSettingsPanel.h"

#include "imgui.h"

namespace Ciri
{
    RendererSettingsPanel::RendererSettingsPanel(const S<Renderer>& renderer)
    {
        SetRenderer(renderer);
    }

    void RendererSettingsPanel::OnUIRender()
    {
        ImGui::Begin("Renderer Settings");
        auto &shaders = m_Renderer->GetShaderLibrary()->GetShaderList();
        if (ImGui::BeginCombo("Shading", shaders[m_Renderer->GetCurrentShader()]->name, 0))
        {
            for (auto &pair : shaders)
            {
                Ciri::Shader *shader = pair.second;
                const bool is_selected = (m_Renderer->GetCurrentShader() == shader->type);
                if (ImGui::Selectable(shader->name, is_selected))
                    m_Renderer->SetCurrentShader(shader->type);
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::Text("[Z]  - Cycle Shader");
        ImGui::Text("[F1] - Hide UI");
        ImGui::End();
    }
}