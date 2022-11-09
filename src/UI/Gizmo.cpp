#include "Gizmo.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Ciri
{
    Gizmo::Gizmo(const S<Camera> &camera) : m_Camera(camera)
    {
    }

    void Gizmo::OnUIRender()
    {
        if (m_SelectedEntity.IsValid() && m_SelectedEntity.HasComponent<TransformComponent>())
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(m_Rect.x, m_Rect.y, m_Rect.z, m_Rect.w);
            TransformComponent &tc = m_SelectedEntity.GetComponent<TransformComponent>();
            glm::mat4 node_transform = tc.Transform.Compose();
            ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMat()), glm::value_ptr(m_Camera->GetProjectionMat()),
                                 ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE | ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL,
                                 glm::value_ptr(node_transform));

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(node_transform, translation, rotation, scale);
                tc.Transform.Translation = translation;
                glm::vec3 deltaRotation = rotation - tc.Transform.Rotation;
                tc.Transform.Rotation += deltaRotation;
                tc.Transform.Scale = scale;
            }
        }
    }
}