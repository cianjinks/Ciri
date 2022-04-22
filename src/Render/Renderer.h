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
        ShaderType m_CurrentShader = ShaderType::BASE_COLOR;

        uint32_t m_GBuffer;

        // Render Targets
        static const uint32_t s_NumRenderTargets;
        uint32_t *m_RenderTargets;
        uint32_t m_GDepthTexture;
        uint32_t m_GBaseColorTexture;
        uint32_t m_GNormalOcclusionTexture;
        uint32_t m_GMetallicRoughnessTexture;
        uint32_t m_GEmissiveTexture;

        // Screen Quad
        uint32_t m_ScreenQuadVAO;
        uint32_t m_ScreenQuadVBO;

    public:
        Renderer(int targetWidth, int targetHeight);
        ~Renderer();

        void Render(Scene *scene, Camera *camera);

        void SetCurrentShader(ShaderType shader) { m_CurrentShader = shader; }
        ShaderType GetCurrentShader() const { return m_CurrentShader; }
        ShaderLibrary *GetShaderLibrary() const { return m_ShaderLib; }

    private:
        void RenderSceneGeometry(Scene *scene, Camera *camera);
        void RenderScreenQuad();

        void BlitDepthBuffer();
    };
}

#endif