#ifndef CIRI_SCENEPANEL_H
#define CIRI_SCENEPANEL_H

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

namespace Ciri
{
    class SceneHierarchyPanel
    {
    private:
        S<Scene> m_Scene;
        Entity m_SelectedEntity;

    public:
        SceneHierarchyPanel(const S<Scene> &scene);
        ~SceneHierarchyPanel() = default;

        void OnUIRender();

        void SetScene(const S<Scene> &scene) { m_Scene = scene; }

        Entity GetSelectedEntity() { return m_SelectedEntity; }

    private:
        void RenderNode(Entity entity);
        const char *GetEntityIcon(Entity entity);
    };
}

#endif