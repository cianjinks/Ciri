#ifndef CIRI_MESHPANEL_H
#define CIRI_MESHPANEL_H

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Render/Material.h"

namespace Ciri
{
    class EntitySettingsPanel
    {
    private:
        Entity m_SelectedEntity;

    public:
        EntitySettingsPanel() = default;
        ~EntitySettingsPanel() = default;

        void OnUIRender();

        void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
    };
}

#endif