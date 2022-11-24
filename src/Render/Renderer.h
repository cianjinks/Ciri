#ifndef CIRI_RENDERER_H
#define CIRI_RENDERER_H

#include "Render/Shader.h"
#include "Render/Material.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Camera.h"

namespace Ciri
{
    class Renderer
    {
    public:
        int32_t TargetWidth, TargetHeight;

    private:
        S<ShaderLibrary> m_ShaderLib;
        ShaderType m_CurrentShader = ShaderType::PHONG_LIGHTING;

        uint32_t m_GBuffer;

        // Render Targets
        static const uint32_t s_NumRenderTargets;
        uint32_t *m_RenderTargets;
        uint32_t m_GDepthTexture;
        uint32_t m_GBaseColorTexture;
        uint32_t m_GNormalOcclusionTexture;
        uint32_t m_GMetallicRoughnessTexture;
        uint32_t m_GEmissiveTexture;

        // Viewport Framebuffer
        uint32_t m_ViewportFB;
        uint32_t m_ViewportColorTexture;
        uint32_t m_ViewportDepthTexture;

        // Viewport Quad
        uint32_t m_ScreenQuadVAO;
        uint32_t m_ScreenQuadVBO;

        // Lighting Constants
        static const uint32_t s_MaxPointLights;
        static const uint32_t s_MaxSpotLights;

    public:
        Renderer(int32_t targetWidth, int32_t targetHeight);
        ~Renderer();

        void OnEvent(Event &event);

        /* Currently the renderer holds the viewport framebuffer and renders to it. But sometimes we want to render to default framebuffer instead. */
        /* For now add a toggle bool. In the future it would make sense to pass in the render target to the renderer. */
        void RenderScene(const S<Scene> &scene, const S<Camera> &camera, bool viewport);

        void Resize(int32_t width, int32_t height);

        void SetCurrentShader(ShaderType shader) { m_CurrentShader = shader; }
        void CycleShader();
        ShaderType GetCurrentShader() const { return m_CurrentShader; }
        const S<ShaderLibrary> &GetShaderLibrary() const { return m_ShaderLib; }

        uint32_t GetViewportTexture() const { return m_ViewportColorTexture; }

        void PrintDeviceInfo()
        {
            CIRI_LOG("Renderer Device Info:");
            CIRI_LOG("  Vendor - {}", glGetString(GL_VENDOR));
            CIRI_LOG("  Renderer - {}", glGetString(GL_RENDERER));
            CIRI_LOG("  Version - {}", glGetString(GL_VERSION));
        }

    private:
        void RenderSceneGeometry(const S<Scene> &scene, const S<Camera> &camera);
        void RenderLighting(const S<Scene> &scene, const S<Camera> &camera);
        void RenderScreenQuad();

        void BlitDepthBuffer(uint32_t framebuffer);

        void ResizeBuffers();
    };
}

#endif