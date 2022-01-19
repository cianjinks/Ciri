#include "Material.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ciri
{
    Material::Material() {}

    MaterialLibrary::MaterialLibrary()
    {
        stbi_set_flip_vertically_on_load(true);
    }

    Material *MaterialLibrary::GetMaterial(const char *name)
    {
        if (m_MaterialList.find(name) != m_MaterialList.end())
        {
            return m_MaterialList[name];
        }
        return nullptr;
    }

    Material *MaterialLibrary::CreateMaterial(const char *name, glm::vec3 baseColor,
                                              MaterialSettings settings,
                                              const char *baseColorTexture,
                                              const char *normalTexture,
                                              const char *metallicTexture,
                                              const char *roughnessTexture,
                                              const char *emissiveTexture,
                                              const char *occlusionTexture,
                                              float subsurface,
                                              float metallic,
                                              float specular,
                                              float specularTint,
                                              float roughness,
                                              float anisotropic,
                                              float sheen,
                                              float sheenTint,
                                              float clearcoat,
                                              float clearcoatGloss)
    {
        Material *material = new Material();

        material->baseColor = baseColor;
        RegisterTexture(baseColorTexture, GL_TEXTURE0, &material->baseColorTextureID, settings);
        RegisterTexture(normalTexture, GL_TEXTURE1, &material->normalTextureID, settings);
        RegisterTexture(metallicTexture, GL_TEXTURE2, &material->metallicTextureID, settings);
        RegisterTexture(roughnessTexture, GL_TEXTURE3, &material->roughnessTextureID, settings);
        RegisterTexture(emissiveTexture, GL_TEXTURE4, &material->emissiveTextureID, settings);
        RegisterTexture(occlusionTexture, GL_TEXTURE5, &material->occlusionTextureID, settings);

        material->subsurface = subsurface;
        material->metallic = metallic;
        material->specular = specular;
        material->specularTint = specularTint;
        material->roughness = roughness;
        material->anisotropic = anisotropic;
        material->sheen = sheen;
        material->sheenTint = sheenTint;
        material->clearcoat = clearcoat;
        material->clearcoatGloss = clearcoatGloss;
        material->settings = settings;

        m_MaterialList[name] = material;
        return material;
    }

    void MaterialLibrary::RegisterTexture(const char *filepath, uint32_t texture_index, uint32_t *texture_id, MaterialSettings settings)
    {
        if (filepath)
        {
            if (m_TextureList.find(filepath) == m_TextureList.end())
            {
                if (!CompileTexture(filepath, texture_index, texture_id, settings))
                {
                    std::cout << "Failed to compile texture: " << filepath << std::endl;
                }
            }
            else
            {
                *texture_id = m_TextureList[filepath];
            }
        }
    }

    bool MaterialLibrary::CompileTexture(const char *filepath, uint32_t texture_index, uint32_t *texture_id, MaterialSettings settings)
    {
        int32_t w, h;
        int32_t nrChannels;

        uint8_t *image = stbi_load(filepath, &w, &h, &nrChannels, STBI_default);
        if (!image)
        {
            std::cerr << "Failed to read texture: " << filepath << std::endl;
            return false;
        }
        std::cout << "Read texture: " << filepath << ", w = " << w << ", h = " << h << ", nrChannels = " << nrChannels << std::endl;

        glGenTextures(1, texture_id);
        glActiveTexture(texture_index);
        glBindTexture(GL_TEXTURE_2D, *texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (settings.mipmap)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            if (settings.mipmap)
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            if (settings.mipmap)
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Invalid number of channels for texture!" << std::endl;
            return false;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);

        m_TextureList[filepath] = *texture_id;
        return true;
    }
};