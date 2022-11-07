#ifndef CIRI_GIZMO_H
#define CIRI_GIZMO_H

#include "Scene/Camera.h"
#include "Scene/Scene.h"

#include <glm/glm.hpp>

namespace Ciri
{
    class Gizmo
    {
    private:
        S<Camera> m_Camera;
        S<SceneNode> m_SelectedNode = nullptr;

        glm::vec4 m_Rect = glm::vec4(0.0f);

    public:
        Gizmo(const S<Camera> &camera);
        ~Gizmo() = default;

        void OnUIRender();

        void SetSelectedNode(S<SceneNode> node) { m_SelectedNode = node; }
        void SetRect(glm::vec2 pos, glm::vec2 scale)
        {
            m_Rect.x = pos.x, m_Rect.y = pos.y;
            m_Rect.z = scale.x, m_Rect.w = scale.y;
        }
    };
}

#endif