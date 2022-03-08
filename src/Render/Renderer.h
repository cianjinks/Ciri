#ifndef CIRI_RENDERER_H
#define CIRI_RENDERER_H

#include "Render/Shader.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"

namespace Ciri
{
    class Renderer
    {
    private:
        ShaderLibrary *m_ShaderLib;

    public:
        Renderer();
        ~Renderer();
        void Render(Scene *scene, Camera *camera);
    };
}

#endif