#ifndef CIRI_VIEWPORT_H
#define CIRI_VIEWPORT_H

#include "Render/Renderer.h"
#include "Window/Window.h"
#include "Window/Event.h"
#include "UI/Gizmo.h"

#include <glm/glm.hpp>

namespace Ciri
{
    class Viewport
    {
    private:
        S<Renderer> m_Renderer;
        S<Gizmo> m_Gizmo;

        glm::vec2 m_ViewportPosition;
        glm::vec2 m_ViewportSize;

        glm::vec2 m_ViewportOffset;

    public:
        Viewport(const S<Camera> &camera, const S<Renderer> &renderer);
        ~Viewport() = default;

        void OnUIRender();
        void OnUpdate();

        glm::vec2 GetPosition() const { return m_ViewportPosition; }
        glm::vec2 GetSize() const { return m_ViewportSize; }

        const S<Gizmo> &GetGizmo() const { return m_Gizmo; }

        /* Return whether a point is within the viewport. */
        bool InViewport(glm::vec2 pos);
    };
}

#endif