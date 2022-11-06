#ifndef CIRI_UI_H
#define CIRI_UI_H

// Include ImGUI before Glad
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "imgui.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Window/Window.h"
#include "Window/Event.h"

namespace Ciri
{
    class UI
    {
    private:
        static bool s_HideUI;

    public:
        static void Init(const S<Window> &window);
        static void PreRender();
        static void PostRender();

        static bool WantInput();

        static void OnEvent(Event &event);
    };
}

#endif