#ifndef CIRI_UI_H
#define CIRI_UI_H

#include "Window.h"

namespace Ciri
{
    class UI
    {
    public:
        static void Init(const S<Window>& window);
        static void PreRender();
        static void PostRender();

        static bool WantInput();
    };
}

#endif