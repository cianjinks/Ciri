#include "Viewport.h"

#include "imgui.h"

namespace Ciri
{
    Viewport::Viewport(const S<Camera> &camera, const S<Renderer> &renderer)
        : m_Renderer(renderer)
    {
        m_Gizmo = CreateS<Gizmo>(camera);
    }

    void Viewport::OnUIRender()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 local_pos = ImGui::GetCursorPos();
        m_ViewportSize.x = size.x, m_ViewportSize.y = size.y;
        m_ViewportPosition.x = pos.x, m_ViewportPosition.y = pos.y;
        m_ViewportOffset.x = local_pos.x, m_ViewportOffset.y = local_pos.y;
        m_Renderer->Resize((uint32_t)size.x, (uint32_t)size.y);

        ImGui::Image(reinterpret_cast<void *>(m_Renderer->GetViewportTexture()), ImVec2{size.x, size.y}, ImVec2{0, 1}, ImVec2{1, 0});

        m_Gizmo->SetRect(m_ViewportPosition + m_ViewportOffset, m_ViewportSize);
        m_Gizmo->OnUIRender();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Viewport::OnUpdate()
    {
        bool mouse_in_viewport = InViewport({ImGui::GetMousePos().x, ImGui::GetMousePos().y});
        bool right_click = Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        if (mouse_in_viewport && right_click && !Window::Get()->IsCursorCaptured())
        {
            Window::Get()->CaptureCursor();
        }
        else if (!right_click)
        {
            Window::Get()->ReleaseCursor();
            Window::Get()->SetMouseStatus(true);
        }
    }

    bool Viewport::InViewport(glm::vec2 pos)
    {
        if (pos.x >= (m_ViewportPosition.x + m_ViewportOffset.x) && pos.x <= (m_ViewportPosition.x + m_ViewportSize.x + m_ViewportOffset.x))
        {
            if (pos.y >= (m_ViewportPosition.y + m_ViewportOffset.y) && pos.y <= (m_ViewportPosition.y + m_ViewportSize.y + m_ViewportOffset.y))
            {
                return true;
            }
        }
        return false;
    }
}