#ifndef CIRI_APPLICATION_H
#define CIRI_APPLICATION_H

#include "UI/UI.h"
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

    public:
        Application();
        ~Application() = default;

        void Run();
        void OnEvent(Event& event);
        void OnUIRender();

        void DefineScene();
    };
}

#endif