#include "Gizmo.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Ciri
{
    Gizmo::Gizmo(const S<Camera> &camera) : m_Camera(camera) {}

    void Gizmo::OnUIRender()
    {
        if (m_SelectedNode)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
            ImGuiIO &io = ImGui::GetIO();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
            glm::mat4 node_transform = glm::translate(glm::mat4(1.0f), m_SelectedNode->Position) * glm::toMat4(glm::quat(m_SelectedNode->Rotation)) * glm::scale(glm::mat4(1.0f), m_SelectedNode->Scale); /* TODO: Create GetTransform function for node and use it in the renderer. */
            ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMat()), glm::value_ptr(m_Camera->GetProjectionMat()),
                                 ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE | ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL,
                                 glm::value_ptr(node_transform));

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(node_transform, translation, rotation, scale);
                m_SelectedNode->Position = translation;
                glm::vec3 deltaRotation = rotation - m_SelectedNode->Rotation;
                m_SelectedNode->Rotation += deltaRotation;
                m_SelectedNode->Scale = scale;
            }
        }
    }
}