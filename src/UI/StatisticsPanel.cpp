#include "StatisticsPanel.h"

#include "imgui.h"
#include <glad/glad.h>

namespace Ciri
{
    StatisticsPanel::StatisticsPanel(const S<Scene> &scene)
    {
        SetScene(scene);
    }

    void StatisticsPanel::OnUIRender()
    {
        ImGui::Begin("Statistics");
        ImGui::Text("Version: OpenGL %s", glGetString(GL_VERSION));
        ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Separator();
        // ImGui::Text("Mesh Count: %d", m_Scene->GetMeshCount());
        // ImGui::Text("Triangles: %d", m_Scene->GetTotalTriCount());
        float dt = Window::Get()->GetTimeStep();
        ImGui::Text("Frame Time: %fms", dt * 1000);
        ImGui::Text("FPS: %f", 1.0f / dt);
        ImGui::End();
    }
}