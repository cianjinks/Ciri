#ifndef CIRI_UI_H
#define CIRI_UI_H

#include "Window/Window.h"
#include "Window/Event.h"

namespace Ciri
{
    class UI
    {
    private:
        static bool s_HideUI;
        
    public:
        static void Init(const S<Window>& window);
        static void PreRender();
        static void PostRender();

        static bool WantInput();

        static void Disable() { s_HideUI = true; }
        static void Enable() { s_HideUI = false; }
        static void ToggleActive() { s_HideUI = !s_HideUI; }
        static bool IsActive() { return !s_HideUI; }

        static void OnEvent(Event& event);
    };
}

#endif