#ifndef CIRI_APPLICATION_H
#define CIRI_APPLICATION_H

#include "UI/UI.h"
#include "UI/Viewport.h"
#include "UI/StatisticsPanel.h"
#include "UI/SceneHierarchyPanel.h"
#include "UI/EntitySettingsPanel.h"
#include "UI/MaterialLibraryPanel.h"
#include "UI/MaterialSettingsPanel.h"
#include "UI/RendererSettingsPanel.h"
#include "Window/Window.h"
#include "Render/Renderer.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Scene/Component.h"
#include "Mesh/Primitive.h"

namespace Ciri
{
    class Application
    {
    public:
        std::string Name = "Rendering Engine";

    private:
        S<Window> m_Window;
        S<Renderer> m_Renderer;
        S<Scene> m_Scene;
        S<Camera> m_Camera;

        // Viewport
        U<Viewport> m_Viewport;

        // UI Panels
        U<StatisticsPanel> m_StatisticsPanel;
        U<SceneHierarchyPanel> m_SceneHierarchyPanel;
        U<EntitySettingsPanel> m_EntitySettingsPanel;
        U<MaterialLibraryPanel> m_MaterialLibraryPanel;
        U<MaterialSettingsPanel> m_MaterialSettingsPanel;
        U<RendererSettingsPanel> m_RendererSettingsPanel;

        // TEMP
        std::vector<Entity> animated_elk;

    public:
        Application();
        ~Application() = default;

        void Run();
        void OnEvent(Event &event);
        void OnUIRender();

        void DefineUI();
        void DefineScene();
        void UpdateScene(float dt);
    };
}

#endif