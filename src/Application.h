#ifndef CIRI_APPLICATION_H
#define CIRI_APPLICATION_H

#include "UI/UI.h"
#include "UI/StatisticsPanel.h"
#include "UI/SceneHierarchyPanel.h"
#include "UI/MeshSettingsPanel.h"
#include "UI/MaterialLibraryPanel.h"
#include "UI/MaterialSettingsPanel.h"
#include "UI/RendererSettingsPanel.h"
#include "Window/Window.h"
#include "Render/Renderer.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Mesh/Primitive.h"

namespace Ciri
{
    class Application
    {
    public:
        std::string Name = "Ciri";

    private:
        S<Window> m_Window;
        S<Renderer> m_Renderer;
        S<Scene> m_Scene;
        S<Camera> m_Camera;

        // UI Panels
        U<StatisticsPanel> m_StatisticsPanel;
        U<SceneHierarchyPanel> m_SceneHierarchyPanel;
        U<MeshSettingsPanel> m_MeshSettingsPanel;
        U<MaterialLibraryPanel> m_MaterialLibraryPanel;
        U<MaterialSettingsPanel> m_MaterialSettingsPanel;
        U<RendererSettingsPanel> m_RendererSettingsPanel;

    public:
        Application();
        ~Application() = default;

        void Run();
        void OnEvent(Event& event);
        void OnUIRender();

        void DefineUI();
        void DefineScene();
    };
}

#endif