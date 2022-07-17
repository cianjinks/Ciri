#ifndef CIRI_INPUT_H
#define CIRI_INPUT_H

#include "Window.h"

/*
 * Static input class for polling key and mouse state.
 * This could just be implemented by Window, but to avoid passing Window around 
 * everywhere it gets passed into Input::Init() at startup and then Input can be 
 * used anywhere. It probably makes more sense to just make Window static in the future :/
*/

namespace Ciri
{
    class Input
    {
    public:
        S<Window> WindowRef;

    public:
        static U<Input> s_InputInstance;
        static void Init(S<Window> window);

        static bool IsKeyPressed(int key);
		static bool IsMouseButtonPressed(int button);
		static double GetMouseX();
		static double GetMouseY();
    };
}

#endif