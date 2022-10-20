#ifndef CIRI_GIZMO_H
#define CIRI_GIZMO_H

#include "Scene/Camera.h"
#include "Scene/Scene.h"

namespace Ciri
{
    class Gizmo
    {
    private:
        S<Camera> m_Camera;
        S<SceneNode> m_SelectedNode = nullptr;

    public:
        Gizmo(const S<Camera> &camera);
        ~Gizmo() = default;

        void OnUIRender();

        void SetSelectedNode(S<SceneNode> node) { m_SelectedNode = node; }
    };
}

#endif