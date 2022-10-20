#include "Renderer.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Ciri
{
    const uint32_t Renderer::s_NumRenderTargets = 4;

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
            CIRI_ASSERT(false, "Geometry Buffer is incomplete!");
        }

        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Output Quad
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
        TargetWidth = width;
        TargetHeight = height;

        ResizeBuffers();

        glViewport(0, 0, TargetWidth, TargetHeight);
    }

    void Renderer::RenderScene(const S<Scene> &scene, const S<Camera> &camera)
    {
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render Pipeline
        // 1. Geometry Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawBuffers(s_NumRenderTargets, m_RenderTargets);

        m_ShaderLib->BindShader(ShaderType::GEOMETRY_BUFFER);
        RenderSceneGeometry(scene, camera);
        m_ShaderLib->BindShader(ShaderType::NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Lighting?

        // 3. Output Result
        m_ShaderLib->BindShader(m_CurrentShader);
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
        BlitDepthBuffer();
    }

    void Renderer::RenderSceneGeometry(const S<Scene> &scene,
                                       const S<Camera> &camera)
    {
        camera->RecalcVP();
        glm::mat4 proj = camera->GetProjectionMat();
        glm::mat4 view = camera->GetViewMat();

        std::stack<RenderStackItem> renderStack;
        S<SceneNode> root = scene->GetRoot();
        renderStack.push({root, root->Position, glm::toMat4(glm::quat(root->Rotation)), root->Scale});

        while (!renderStack.empty())
        {
            RenderStackItem &currentItem = renderStack.top();
            S<SceneNode> currentNode = currentItem.node;
            glm::vec3 currentNodePosition = currentItem.position;
            glm::mat4 currentNodeRotation = currentItem.rotation;
            glm::vec3 currentNodeScale = currentItem.scale;
            if (currentNode->NodeMesh) // No mesh means this is just a container
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, currentNodePosition);
                model = model * currentNodeRotation;
                /* TODO: The scene hierarchy does not actually work correctly.
                 * Consider parenting cube to cube and offset positon then rotation.
                 *      Also scale is incorrect when trying to scale sub-objects on
                 * sponza, because these objects have position (0, 0, 0) but their
                 * vertices are actually relative to the (0, 0, 0) of the entire
                 * sponza model. I presume this is not fixable :(
                 */
                model = glm::scale(model, currentNodeScale);
                glm::mat4 mvp = proj * view * model;
                m_ShaderLib->SetMat4f("u_MVP", glm::value_ptr(mvp));

                S<Material> material = currentNode->NodeMaterial;
                m_ShaderLib->SetVec3f("u_BaseColor", material->baseColor);
                m_ShaderLib->SetInt1i("u_BaseColorTexture", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->baseColorTextureID);
                m_ShaderLib->SetInt1i("u_NormalTexture", 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, material->normalTextureID);
                m_ShaderLib->SetInt1i("u_MetallicRoughnessTexture", 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, material->metallicRoughnessTextureID);
                m_ShaderLib->SetInt1i("u_OcclusionTexture", 3);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, material->occlusionTextureID);
                m_ShaderLib->SetInt1i("u_EmissiveTexture", 4);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, material->emissiveTextureID);

                S<Mesh> mesh = currentNode->NodeMesh;
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

            renderStack.pop();
            for (S<SceneNode> node : currentNode->Children)
            {
                renderStack.push({node, currentNodePosition + node->Position,
                                  currentNodeRotation * glm::toMat4(glm::quat(node->Rotation)),
                                  currentNodeScale * node->Scale});
            }
        }
    }

    void Renderer::RenderScreenQuad()
    {
        glBindVertexArray(m_ScreenQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void Renderer::BlitDepthBuffer()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, TargetWidth, TargetHeight, 0, 0, TargetWidth,
                          TargetHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void Renderer::ResizeBuffers()
    {
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
    }
} // namespace Ciri