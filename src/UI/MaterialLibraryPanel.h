#ifndef CIRI_MATLIBPANEL_H
#define CIRI_MATLIBPANEL_H

#include "Render/Material.h"

namespace Ciri
{
    class MaterialLibraryPanel
    {
    private:
        MaterialLibrary& m_MaterialLibrary;
        S<Material> m_SelectedMaterial = nullptr;

    public:
        MaterialLibraryPanel(MaterialLibrary& matlib);
        MaterialLibraryPanel() = default;

        void OnUIRender();

        void SetMaterialLibrary(MaterialLibrary& matlib) { m_MaterialLibrary = matlib; }

        S<Material> GetSelectedMaterial() { return m_SelectedMaterial; }
    };
}

#endif