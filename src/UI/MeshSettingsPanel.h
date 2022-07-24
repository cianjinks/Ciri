#ifndef CIRI_MESHPANEL_H
#define CIRI_MESHPANEL_H

#include "Scene/Scene.h"
#include "Render/Material.h"

namespace Ciri
{
    class MeshSettingsPanel
    {
    private:
        SceneNode* m_SelectedNode = nullptr;
        MaterialLibrary& m_MaterialLibrary;

    public:
        MeshSettingsPanel(MaterialLibrary& matlib);
        ~MeshSettingsPanel() = default;

        void OnUIRender();

        void SetSelectedNode(SceneNode* node) { m_SelectedNode = node; }
    };
}

#endif