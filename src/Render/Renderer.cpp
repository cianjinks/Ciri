#include "Renderer.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ciri
{
    Renderer::Renderer()
    {
        m_ShaderLib = new ShaderLibrary();
    }

    Renderer::~Renderer()
    {
        delete m_ShaderLib;
    }

    void Renderer::Render(Scene *scene, Camera *camera)
    {
        // Render Pipeline
        // 1. Geometry Buffer
        uint32_t gBuffer;
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        // TODO: Implement multiple render targets

        uint32_t buffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, buffers);
        DrawScene(scene, camera);
    }

    void Renderer::DrawScene(Scene *scene, Camera *camera)
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

                m_ShaderLib->BindShader(CurrentShader);
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
                m_ShaderLib->BindShader(ShaderType::NONE);
            }

            renderStack.pop();
            for (SceneNode *node : currentNode->Children)
            {
                renderStack.push({node, currentNodePosition + node->Position, currentNodeScale * node->Scale});
            }
        }
    }
}