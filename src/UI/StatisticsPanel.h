#ifndef CIRI_STATPANEL_H
#define CIRI_STATPANEL_H

#include "Scene/Scene.h"

namespace Ciri
{
    class StatisticsPanel
    {
    private:
        S<Scene> m_Scene;

    public:
        StatisticsPanel(const S<Scene>& scene);
        ~StatisticsPanel() = default;

        void OnUIRender();

        void SetScene(const S<Scene>& scene) { m_Scene = scene; }
    };
}

#endif