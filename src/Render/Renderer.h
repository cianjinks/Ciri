#ifndef CIRI_RENDERER_H
#define CIRI_RENDERER_H

#include "Render/Shader.h"
#include "Render/Material.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"

namespace Ciri
{
    struct RenderStackItem
    {
        SceneNode *node;
        glm::vec3 position;
        glm::vec3 scale;
    };

    class Renderer
    {
    public:
        ShaderType CurrentShader = ShaderType::FLAT_NORMAL;

    private:
        ShaderLibrary *m_ShaderLib;

    public:
        Renderer();
        ~Renderer();
        void Render(Scene *scene, Camera *camera);
    };
}

#endif