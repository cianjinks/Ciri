#ifndef CIRI_APPLICATION_H
#define CIRI_APPLICATION_H

#include "Window/Window.h"
#include "Window/Event.h"
#include "Window/Input.h"
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
        float DT = 0.0f;

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

        void DefineScene();
    };
}

#endif