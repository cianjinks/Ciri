#include "MaterialLibraryPanel.h"

#include "imgui.h"
#include "IconsForkAwesome.h"

namespace Ciri
{
    MaterialLibraryPanel::MaterialLibraryPanel(MaterialLibrary& matlib)
        : m_MaterialLibrary(matlib)
    {
    }

    void MaterialLibraryPanel::OnUIRender()
    {
        ImGui::Begin("Material");
        for (auto &pair : m_MaterialLibrary.GetMaterials())
        {
            std::string name = pair.first;
            Material *material = pair.second;
            std::string text = ICON_FK_CODEPEN + std::string(" ") + name;
            if (ImGui::Selectable(text.c_str(), m_SelectedMaterial == material))
            {
                m_SelectedMaterial = material;
            }
        }
        ImGui::End();
    }
}