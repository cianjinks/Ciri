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
        S<ShaderLibrary> m_ShaderLib;
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

        void OnEvent(Event& event);

        void RenderScene(const S<Scene> &scene, const S<Camera> &camera);

        void SetCurrentShader(ShaderType shader) { m_CurrentShader = shader; }
        void CycleShader();
        ShaderType GetCurrentShader() const { return m_CurrentShader; }
        const S<ShaderLibrary> &GetShaderLibrary() const { return m_ShaderLib; }

        void PrintDeviceInfo()
        {
            CIRI_LOG("Renderer Device Info:");
            CIRI_LOG("  Vendor - {}", glGetString(GL_VENDOR));
            CIRI_LOG("  Renderer - {}", glGetString(GL_RENDERER));
            CIRI_LOG("  Version - {}", glGetString(GL_VERSION));
        }

    private:
        void RenderSceneGeometry(const S<Scene> &scene, const S<Camera> &camera);
        void RenderScreenQuad();

        void BlitDepthBuffer();
    };
}

#endif