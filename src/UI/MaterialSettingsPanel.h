#ifndef CIRI_MATSETTINGSPANEL_H
#define CIRI_MATSETTINGSPANEL_H

#include "Render/Material.h"

namespace Ciri
{
    class MaterialSettingsPanel
    {
    private:
        S<Material> m_SelectedMaterial = nullptr;
        MaterialLibrary& m_MaterialLibrary;

    public:
        MaterialSettingsPanel(MaterialLibrary& matlib);
        ~MaterialSettingsPanel() = default;

        void OnUIRender();

        void SetSelectedNode(S<Material> material) { m_SelectedMaterial = material; }
    
    private:
        void CreateTextureCombo(std::string name, std::string &material_tex_name, uint32_t &material_tex_id);
    };
}

#endif