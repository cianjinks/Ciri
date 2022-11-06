#ifndef CIRI_VIEWPORT_H
#define CIRI_VIEWPORT_H

#include "Render/Renderer.h"
#include "Window/Window.h"
#include "Window/Event.h"

#include <glm/glm.hpp>

namespace Ciri
{
    class Viewport
    {
    private:
        S<Renderer> m_Renderer;

        glm::vec2 m_ViewportPosition;
        glm::vec2 m_ViewportSize;

        glm::vec2 m_ViewportOffset;

    public:
        Viewport(const S<Renderer>& renderer);
        ~Viewport() = default;

        void OnUIRender();
        void OnUpdate();

        /* Return whether a point is within the viewport. */
        bool InViewport(glm::vec2 pos);
    };
}

#endif