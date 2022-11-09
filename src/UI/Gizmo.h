#ifndef CIRI_GIZMO_H
#define CIRI_GIZMO_H

#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include <glm/glm.hpp>

namespace Ciri
{
    class Gizmo
    {
    private:
        S<Camera> m_Camera;
        Entity m_SelectedEntity;

        glm::vec4 m_Rect = glm::vec4(0.0f);

    public:
        Gizmo(const S<Camera> &camera);
        ~Gizmo() = default;

        void OnUIRender();

        void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
        void SetRect(glm::vec2 pos, glm::vec2 scale)
        {
            m_Rect.x = pos.x, m_Rect.y = pos.y;
            m_Rect.z = scale.x, m_Rect.w = scale.y;
        }
    };
}

#endif