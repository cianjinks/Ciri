#ifndef CIRI_SCENEPANEL_H
#define CIRI_SCENEPANEL_H

#include "Scene/Scene.h"

namespace Ciri
{
    class SceneHierarchyPanel
    {
    private:
        S<Scene> m_Scene;
        SceneNode* m_SelectedNode = nullptr;

    public:
        SceneHierarchyPanel(const S<Scene>& scene);
        ~SceneHierarchyPanel() = default;

        void OnUIRender();

        void SetScene(const S<Scene>& scene) { m_Scene = scene; }

        SceneNode* GetSelectedNode() { return m_SelectedNode; }

    private:
        void RenderNodeHierarchy(SceneNode *root, int ptr_id);
    };
}

#endif