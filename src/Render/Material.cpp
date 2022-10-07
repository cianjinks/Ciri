#include "Material.h"

#include "stb_image.h"
#include <glad/glad.h>

namespace Ciri
{
    Material::Material() {}

    MaterialLibrary::MaterialLibrary() {}

    S<Material> MaterialLibrary::GetMaterial(std::string name)
    {
        if (m_MaterialList.find(name) != m_MaterialList.end())
        {
            return m_MaterialList[name];
        }
        return nullptr;
    }

    S<Material> MaterialLibrary::CreateMaterial(MaterialInfo &info,
                                                glm::vec3 baseColor,
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
        S<Material> material = CreateS<Material>();

        material->baseColor = baseColor;
        RegisterTexture(info.baseColorFilepath, GL_TEXTURE0, &material->baseColorTextureID, info);
        RegisterTexture(info.normalFilepath, GL_TEXTURE1, &material->normalTextureID, info);
        RegisterTexture(info.metallicRoughnessFilepath, GL_TEXTURE2, &material->metallicRoughnessTextureID, info);
        RegisterTexture(info.occlusionFilepath, GL_TEXTURE3, &material->occlusionTextureID, info);
        RegisterTexture(info.emissiveFilepath, GL_TEXTURE4, &material->emissiveTextureID, info);

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

        material->info = info;

        m_MaterialList[info.name] = material;
        return material;
    }

    void MaterialLibrary::RegisterTexture(std::string filepath, uint32_t texture_index, uint32_t *texture_id, MaterialInfo &info)
    {
        if (!filepath.empty())
        {
            if (m_TextureList.find(filepath) == m_TextureList.end())
            {
                if (!CompileTexture(filepath, texture_index, texture_id, info))
                {
                    CIRI_WARN("Failed to compile texture: {}", filepath);
                }
            }
            else
            {
                *texture_id = m_TextureList[filepath];
            }
        }
    }

    bool MaterialLibrary::CompileTexture(std::string filepath, uint32_t texture_index, uint32_t *texture_id, MaterialInfo &info)
    {
        int32_t w, h;
        int32_t nrChannels;

        stbi_set_flip_vertically_on_load(info.flip);

        uint8_t *image = stbi_load(filepath.c_str(), &w, &h, &nrChannels, STBI_default);
        if (!image)
        {
            CIRI_WARN("Failed to read texture: {}", filepath);
            return false;
        }
        CIRI_LOG("Read texture: {}, w = {:d}, h = {:d}, nrChannels = {:d}", filepath, w, h, nrChannels);

        glGenTextures(1, texture_id);
        glActiveTexture(texture_index);
        glBindTexture(GL_TEXTURE_2D, *texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (info.mipmap)
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
            if (info.mipmap)
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            if (info.mipmap)
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            CIRI_WARN("Invalid number of channels for texture! {}", nrChannels);
            return false;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);

        m_TextureList[filepath] = *texture_id;
        return true;
    }
};