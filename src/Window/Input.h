#ifndef CIRI_INPUT_H
#define CIRI_INPUT_H

#include <glm/glm.hpp>

#include "Window.h"

/*
 * Static input class for polling key and mouse state.
 * This could just be implemented by Window, but I think
 * it makes sense to separate the concepts for more readable code.
*/

namespace Ciri
{
    class Input
    {
    public:
        static bool IsKeyPressed(int key);
		static bool IsMouseButtonPressed(int button);
        static glm::dvec2 GetMouseCoord();
		static double GetMouseX();
		static double GetMouseY();
    };
}

#endif