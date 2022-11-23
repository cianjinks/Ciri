#include "Renderer.h"

#include "Scene/Component.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Ciri
{
    const uint32_t Renderer::s_NumRenderTargets = 4;
    const uint32_t Renderer::s_MaxPointLights = 256; /* See `phong.frag`. */

    Renderer::Renderer(int targetWidth, int targetHeight)
        : TargetWidth(targetWidth), TargetHeight(targetHeight)
    {
        m_ShaderLib = CreateS<ShaderLibrary>();

        glEnable(GL_DEPTH_TEST);

        // clang-format off
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };
        // clang-format on

        // Geometry Buffer
        glGenFramebuffers(1, &m_GBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);

        glGenTextures(1, &m_GBaseColorTexture);
        glBindTexture(GL_TEXTURE_2D, m_GBaseColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               m_GBaseColorTexture, 0);

        glGenTextures(1, &m_GNormalOcclusionTexture);
        glBindTexture(GL_TEXTURE_2D, m_GNormalOcclusionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                               m_GNormalOcclusionTexture, 0);

        glGenTextures(1, &m_GMetallicRoughnessTexture);
        glBindTexture(GL_TEXTURE_2D, m_GMetallicRoughnessTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                               m_GMetallicRoughnessTexture, 0);

        glGenTextures(1, &m_GEmissiveTexture);
        glBindTexture(GL_TEXTURE_2D, m_GEmissiveTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,
                               m_GEmissiveTexture, 0);

        glGenTextures(1, &m_GDepthTexture);
        glBindTexture(GL_TEXTURE_2D, m_GDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TargetWidth,
                     TargetHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               m_GDepthTexture, 0);

        m_RenderTargets = new uint32_t[s_NumRenderTargets]{
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3};
        glDrawBuffers(s_NumRenderTargets, m_RenderTargets);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            CIRI_ASSERT(false, "Geometry Framebuffer is incomplete!");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Viewport Framebuffer
        glGenFramebuffers(1, &m_ViewportFB);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ViewportFB);

        glGenTextures(1, &m_ViewportColorTexture);
        glBindTexture(GL_TEXTURE_2D, m_ViewportColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               m_ViewportColorTexture, 0);

        glGenTextures(1, &m_ViewportDepthTexture);
        glBindTexture(GL_TEXTURE_2D, m_ViewportDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TargetWidth,
                     TargetHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               m_ViewportDepthTexture, 0);

        /* No need to specify draw buffers because it defaults to single color output? */
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            CIRI_ASSERT(false, "Viewport Framebuffer is incomplete!");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Viewport Quad
        glGenVertexArrays(1, &m_ScreenQuadVAO);
        glGenBuffers(1, &m_ScreenQuadVBO);

        glBindVertexArray(m_ScreenQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_ScreenQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    Renderer::~Renderer() { delete[] m_RenderTargets; }

    void Renderer::OnEvent(Event &event)
    {
        EventType type = event.GetEventType();
        switch (type)
        {
        case EventType::WINDOW_RESIZE:
        {
            WindowResizeEvent &resize_event =
                static_cast<WindowResizeEvent &>(event);
            Resize(resize_event.GetWidth(), resize_event.GetHeight());
            break;
        }
        case EventType::KEY_PRESS:
        {
            KeyEvent &key_event = static_cast<KeyEvent &>(event);
            if (key_event.GetKey() == GLFW_KEY_Z)
            {
                CycleShader();
            }
            break;
        }
        }
    }

    void Renderer::CycleShader()
    {
        /* TODO: Fix this. */
        ShaderType next =
            static_cast<ShaderType>((static_cast<int>(GetCurrentShader()) + 1) %
                                    GetShaderLibrary()->GetShaderCount());
        SetCurrentShader(next);
    }

    void Renderer::Resize(int32_t width, int32_t height)
    {
        if (TargetWidth != width || TargetHeight != height)
        {
            TargetWidth = width;
            TargetHeight = height;

            ResizeBuffers();

            glViewport(0, 0, TargetWidth, TargetHeight);
        }
    }

    void Renderer::RenderScene(const S<Scene> &scene, const S<Camera> &camera, bool viewport)
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render Pipeline
        // 1. Geometry Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawBuffers(s_NumRenderTargets, m_RenderTargets);

        m_ShaderLib->BindShader(ShaderType::GEOMETRY_BUFFER);
        RenderSceneGeometry(scene, camera);
        m_ShaderLib->BindShader(ShaderType::NONE);

        // 2. Lighting Pass + Other things for now
        glBindFramebuffer(GL_FRAMEBUFFER, viewport ? m_ViewportFB : 0);
        m_ShaderLib->BindShader(m_CurrentShader);

        if (m_CurrentShader == ShaderType::PHONG_LIGHTING)
        {
            RenderLighting(scene, camera);
        }
        else if (m_CurrentShader != ShaderType::GEOMETRY_BUFFER)
        {
            camera->RecalcVP();
            glm::mat4 invProjMat = camera->GetInvProjMat();
            glm::mat4 invViewMat = camera->GetInvViewMat();
            m_ShaderLib->SetMat4f("u_InvProjMat", glm::value_ptr(invProjMat));
            m_ShaderLib->SetMat4f("u_InvViewMat", glm::value_ptr(invViewMat));
            m_ShaderLib->SetFloat1f("u_CameraFar", camera->Far);
            m_ShaderLib->SetFloat1f("u_CameraNear", camera->Near);
            // Render Target Textures
            m_ShaderLib->SetInt1i("u_DepthTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_GDepthTexture);
            m_ShaderLib->SetInt1i("u_BaseColorTexture", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_GBaseColorTexture);
            m_ShaderLib->SetInt1i("u_NormalOcclusionTexture", 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_GNormalOcclusionTexture);
            m_ShaderLib->SetInt1i("u_MetallicRoughnessTexture", 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, m_GMetallicRoughnessTexture);
            m_ShaderLib->SetInt1i("u_EmissiveTexture", 4);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, m_GEmissiveTexture);

            RenderScreenQuad();
            glBindTexture(GL_TEXTURE_2D, 0);
            m_ShaderLib->BindShader(ShaderType::NONE);
        }

        // 3. Output Result
        BlitDepthBuffer(viewport ? m_ViewportFB : 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::RenderSceneGeometry(const S<Scene> &scene,
                                       const S<Camera> &camera)
    {
        camera->RecalcVP();
        glm::mat4 proj = camera->GetProjectionMat();
        glm::mat4 view = camera->GetViewMat();

        entt::registry &registry = scene->GetRegistry();
        auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
        for (auto entity : group)
        {
            auto [tc, mc] = group.get<TransformComponent, MeshComponent>(entity);

            glm::mat4 model = tc.Transform.GetWorldMatrix();
            m_ShaderLib->SetMat4f("u_ProjectionMatrix", glm::value_ptr(proj));
            m_ShaderLib->SetMat4f("u_ViewMatrix", glm::value_ptr(view));
            m_ShaderLib->SetMat4f("u_ModelMatrix", glm::value_ptr(model));
            glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
            m_ShaderLib->SetMat3f("u_NormalMatrix", glm::value_ptr(normal_matrix));

            Entity wrap_entity(entity, scene);
            if (wrap_entity.HasComponent<MaterialComponent>())
            {
                auto &mc = wrap_entity.GetComponent<MaterialComponent>();
                S<Material> &material = mc.CMaterial;
                m_ShaderLib->SetVec3f("u_BaseColor", material->spec.baseColor);
                m_ShaderLib->SetInt1i("u_BaseColorTexture", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->spec.baseColorTextureID);
                m_ShaderLib->SetInt1i("u_NormalTexture", 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, material->spec.normalTextureID);
                m_ShaderLib->SetInt1i("u_MetallicRoughnessTexture", 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, material->spec.metallicRoughnessTextureID);
                m_ShaderLib->SetInt1i("u_OcclusionTexture", 3);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, material->spec.occlusionTextureID);
                m_ShaderLib->SetInt1i("u_EmissiveTexture", 4);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, material->spec.emissiveTextureID);
            }

            S<Mesh> mesh = mc.CMesh;
            glBindVertexArray(mesh->m_VAO);

            if (mesh->IsIndexed)
            {
                glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_SHORT, 0);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, 3 * mesh->TriCount);
            }

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Renderer::RenderLighting(const S<Scene> &scene,
                                  const S<Camera> &camera)
    {
        /* GBuffer Textures. */
        m_ShaderLib->SetInt1i("u_DepthTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_GDepthTexture);
        m_ShaderLib->SetInt1i("u_BaseColorTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_GBaseColorTexture);
        m_ShaderLib->SetInt1i("u_NormalOcclusionTexture", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_GNormalOcclusionTexture);

        /* Camera information for computing FragPos from depth. */
        camera->RecalcVP();
        glm::mat4 invProjMat = camera->GetInvProjMat();
        glm::mat4 invViewMat = camera->GetInvViewMat();
        m_ShaderLib->SetMat4f("u_InvProjMat", glm::value_ptr(invProjMat));
        m_ShaderLib->SetMat4f("u_InvViewMat", glm::value_ptr(invViewMat));
        m_ShaderLib->SetVec3f("u_CameraPos", camera->Position);

        /* Uploading lights. */
        int num_point_lights = 0;
        entt::registry &registry = scene->GetRegistry();
        auto group = registry.group<LightComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [lc, tc] = group.get<LightComponent, TransformComponent>(entity);

            if (lc.Type == LightType::POINT)
            {
                if (num_point_lights < s_MaxPointLights)
                {
                    /* TODO: Use UBO.*/
                    std::string uniform_string = "u_PointLights[" + std::to_string(num_point_lights) + "]";
                    std::string uniform_position_string = uniform_string + ".position";
                    std::string uniform_ambient_string = uniform_string + ".ambient";
                    std::string uniform_diffuse_string = uniform_string + ".diffuse";
                    std::string uniform_specular_string = uniform_string + ".specular";
                    m_ShaderLib->SetVec3f(uniform_position_string.c_str(), tc.Transform.GetWorldTranslation());
                    m_ShaderLib->SetVec3f(uniform_ambient_string.c_str(), lc.Ambient);
                    m_ShaderLib->SetVec3f(uniform_diffuse_string.c_str(), lc.Diffuse);
                    m_ShaderLib->SetVec3f(uniform_specular_string.c_str(), lc.Specular);
                }
                num_point_lights++;
            }
        }
        m_ShaderLib->SetInt1i("u_NumPointLights", num_point_lights);
        if (num_point_lights > s_MaxPointLights)
        {
            CIRI_WARN("Failed to render some point lights. Scene contains too many point lights ({}), max is {}!", num_point_lights, s_MaxPointLights);
        }

        /* Output Result. */
        RenderScreenQuad();
        glBindTexture(GL_TEXTURE_2D, 0);
        m_ShaderLib->BindShader(ShaderType::NONE);
    }

    void Renderer::RenderScreenQuad()
    {
        glBindVertexArray(m_ScreenQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void Renderer::BlitDepthBuffer(uint32_t framebuffer)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
        glBlitFramebuffer(0, 0, TargetWidth, TargetHeight, 0, 0, TargetWidth,
                          TargetHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void Renderer::ResizeBuffers()
    {
        /* No need to recreate entire framebuffers? */

        // Geometry Buffer
        glBindTexture(GL_TEXTURE_2D, m_GBaseColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, m_GNormalOcclusionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, m_GMetallicRoughnessTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, m_GEmissiveTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, m_GDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TargetWidth,
                     TargetHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        // Viewport
        glBindTexture(GL_TEXTURE_2D, m_ViewportColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TargetWidth, TargetHeight, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, m_ViewportDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TargetWidth,
                     TargetHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
} // namespace Ciri