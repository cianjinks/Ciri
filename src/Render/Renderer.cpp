#include "Renderer.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ciri
{
    Renderer::Renderer(int targetWidth, int targetHeight)
        : TargetWidth(targetWidth), TargetHeight(targetHeight)
    {
        m_ShaderLib = new ShaderLibrary();

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

        glGenTextures(1, &m_GPositionTexture);
        glBindTexture(GL_TEXTURE_2D, m_GPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TargetWidth, TargetHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GPositionTexture, 0);

        glGenTextures(1, &m_GNormalTexture);
        glBindTexture(GL_TEXTURE_2D, m_GNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TargetWidth, TargetHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GNormalTexture, 0);

        // Color Attachments
        m_RenderTargets = new uint32_t[2]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, m_RenderTargets);

        // Depth Attachment
        glGenRenderbuffers(1, &m_GDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_GDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, TargetWidth, TargetHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_GDepthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            CIRI_ERROR("Geometry Buffer is incomplete!");
        }

        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Output Quad
        glGenVertexArrays(1, &m_ScreenQuadVAO);
        glGenBuffers(1, &m_ScreenQuadVBO);

        glBindVertexArray(m_ScreenQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_ScreenQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    Renderer::~Renderer()
    {
        delete m_ShaderLib;
        delete m_RenderTargets;
    }

    void Renderer::Render(Scene *scene, Camera *camera)
    {
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render Pipeline
        // 1. Geometry Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, m_GPositionTexture);
        glBindTexture(GL_TEXTURE_2D, m_GNormalTexture);
        glDrawBuffers(3, m_RenderTargets);

        m_ShaderLib->BindShader(ShaderType::GEOMETRY_BUFFER);
        RenderSceneGeometry(scene, camera);
        m_ShaderLib->BindShader(ShaderType::NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Lighting?

        // 3. Output Result
        RenderSingleTarget(m_GNormalTexture);
    }

    void Renderer::RenderSceneGeometry(Scene *scene, Camera *camera)
    {
        camera->RecalcMVP();
        glm::mat4 proj = camera->GetProjectionMat();
        glm::mat4 view = camera->GetViewMat();

        std::stack<RenderStackItem> renderStack;
        SceneNode *root = scene->GetRoot();
        renderStack.push({root, root->Position, root->Scale});

        while (!renderStack.empty())
        {
            RenderStackItem &currentItem = renderStack.top();
            SceneNode *currentNode = currentItem.node;
            glm::vec3 currentNodePosition = currentItem.position;
            glm::vec3 currentNodeScale = currentItem.scale;
            if (currentNode->NodeMesh) // No mesh means this is just a container
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, currentNodePosition);
                /* TODO:
                 *      Scale is incorrect when trying to scale sub-objects on sponza,
                 *      because these objects have position (0, 0, 0) but their vertices are actually
                 *      relative to the (0, 0, 0) of the entire sponza model. I presume this is not fixable :(
                 */
                model = glm::scale(model, currentNodeScale);
                glm::mat4 mvp = proj * view * model;

                m_ShaderLib->SetMat4f("u_MVP", glm::value_ptr(mvp));

                Material *material = currentNode->NodeMaterial;
                m_ShaderLib->SetInt1i("u_BaseColorTexture", 0);
                m_ShaderLib->SetVec3f("u_BaseColor", material->baseColor);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->baseColorTextureID);
                m_ShaderLib->SetInt1i("u_NormalTexture", 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, material->normalTextureID);
                m_ShaderLib->SetInt1i("u_MetallicTexture", 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, material->metallicTextureID);
                m_ShaderLib->SetInt1i("u_RoughnessTexture", 3);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, material->roughnessTextureID);
                m_ShaderLib->SetInt1i("u_EmissiveTexture", 4);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, material->emissiveTextureID);
                m_ShaderLib->SetInt1i("u_OcclusionTexture", 5);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, material->occlusionTextureID);

                Mesh *mesh = currentNode->NodeMesh;
                glBindVertexArray(mesh->m_VAO);

                glDrawArrays(GL_TRIANGLES, 0, 3 * mesh->TriCount);

                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            renderStack.pop();
            for (SceneNode *node : currentNode->Children)
            {
                renderStack.push({node, currentNodePosition + node->Position, currentNodeScale * node->Scale});
            }
        }
    }

    void Renderer::RenderSingleTarget(uint32_t target)
    {
        m_ShaderLib->BindShader(ShaderType::SINGLE_TARGET);
        m_ShaderLib->SetInt1i("u_TargetTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, target);
        RenderScreenQuad();
        glBindTexture(GL_TEXTURE_2D, 0);
        m_ShaderLib->BindShader(ShaderType::NONE);
        BlitDepthBuffer();
    }

    void Renderer::RenderMultipleTarget()
    {
        m_ShaderLib->BindShader(ShaderType::MULTIPLE_TARGET);
        m_ShaderLib->SetInt1i("u_PositionTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_GPositionTexture);
        m_ShaderLib->SetInt1i("u_NormalTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_GNormalTexture);
        RenderScreenQuad();
        glBindTexture(GL_TEXTURE_2D, 0);
        m_ShaderLib->BindShader(ShaderType::NONE);
        BlitDepthBuffer();
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
        glBlitFramebuffer(0, 0, TargetWidth, TargetHeight, 0, 0, TargetWidth, TargetHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
}