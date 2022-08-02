#ifndef CIRI_RENDERPANEL_H
#define CIRI_RENDERPANEL_H

#include "Render/Renderer.h"

namespace Ciri
{
    class RendererSettingsPanel
    {
    private:
        S<Renderer> m_Renderer;

    public:
        RendererSettingsPanel(const S<Renderer>& renderer);
        ~RendererSettingsPanel() = default;

        void OnUIRender();

        void SetRenderer(const S<Renderer>& renderer) { m_Renderer = renderer; }
    };
}

#endif