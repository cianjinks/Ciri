#ifndef CIRI_MESHPANEL_H
#define CIRI_MESHPANEL_H

#include "Scene/Scene.h"
#include "Render/Material.h"

#if 0
namespace Ciri
{
    class EntitySettingsPanel
    {
    private:
        S<SceneNode> m_SelectedNode = nullptr;
        MaterialLibrary &m_MaterialLibrary;

    public:
        EntitySettingsPanel(MaterialLibrary &matlib);
        ~EntitySettingsPanel() = default;

        void OnUIRender();

        void SetSelectedNode(S<SceneNode> node) { m_SelectedNode = node; }
    };
}
#endif

#endif