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
        int TargetWidth, TargetHeight;

    private:
        ShaderLibrary *m_ShaderLib;
        uint32_t m_GBuffer;
        uint32_t m_GDepthBuffer;
        uint32_t m_GPositionTexture;
        uint32_t m_GNormalTexture;
        uint32_t *m_RenderTargets;

        uint32_t m_ScreenQuadVAO;
        uint32_t m_ScreenQuadVBO;

    public:
        Renderer(int targetWidth, int targetHeight);
        ~Renderer();
        void Render(Scene *scene, Camera *camera);

    private:
        void RenderSingleTarget(uint32_t target);
        void RenderMultipleTarget();
        void RenderSceneGeometry(Scene *scene, Camera *camera);
        void RenderScreenQuad();

        void BlitDepthBuffer();
    };
}

#endif