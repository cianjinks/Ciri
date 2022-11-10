#include "Material.h"

#include "stb_image.h"
#include <glad/glad.h>

namespace Ciri
{

    S<Material> Material::Create(MaterialSpecification spec)
    {
        return S<Material>(new Material(spec));
    }

    MaterialLibrary::MaterialLibrary()
    {
        CreateDefaultTexture();
        CreateDefaultMaterial();
    }

    void MaterialLibrary::CreateDefaultTexture()
    {
        glGenTextures(1, &m_DefaultTextureID);
        glBindTexture(GL_TEXTURE_2D, m_DefaultTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int32_t w = 1, h = 1, nrChannels = 3;
        uint8_t *image = new uint8_t[3]{255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        delete[] image;
        glBindTexture(GL_TEXTURE_2D, 0);
        m_TextureList["default"] = m_DefaultTextureID;
    }

    void MaterialLibrary::CreateDefaultMaterial()
    {
        MaterialSpecification default_mat_spec;
        default_mat_spec.name = "default";
        default_mat_spec.baseColorTextureID = m_DefaultTextureID;
        default_mat_spec.normalTextureID = m_DefaultTextureID;
        default_mat_spec.metallicRoughnessTextureID = m_DefaultTextureID;
        default_mat_spec.occlusionTextureID = m_DefaultTextureID;
        default_mat_spec.emissiveTextureID = m_DefaultTextureID;
        m_DefaultMaterial = CreateMaterial(default_mat_spec);
    }

    S<Material> MaterialLibrary::GetMaterial(std::string name)
    {
        if (m_MaterialList.find(name) != m_MaterialList.end())
        {
            return m_MaterialList[name];
        }
        return nullptr;
    }

    S<Material> MaterialLibrary::CreateMaterial(MaterialSpecification &spec)
    {
        RegisterTexture(spec.baseColorFilepath, GL_TEXTURE0, &spec.baseColorTextureID, spec.mipmap, spec.flip);
        RegisterTexture(spec.normalFilepath, GL_TEXTURE1, &spec.normalTextureID, spec.mipmap, spec.flip);
        RegisterTexture(spec.metallicRoughnessFilepath, GL_TEXTURE2, &spec.metallicRoughnessTextureID, spec.mipmap, spec.flip);
        RegisterTexture(spec.occlusionFilepath, GL_TEXTURE3, &spec.occlusionTextureID, spec.mipmap, spec.flip);
        RegisterTexture(spec.emissiveFilepath, GL_TEXTURE4, &spec.emissiveTextureID, spec.mipmap, spec.flip);

        S<Material> material = Material::Create(spec);
        m_MaterialList[spec.name] = material;
        return material;
    }

    void MaterialLibrary::RegisterTexture(std::string filepath, uint32_t texture_index, uint32_t *texture_id, bool mipmap, bool flip)
    {
        if (!filepath.empty())
        {
            if (m_TextureList.find(filepath) == m_TextureList.end())
            {
                if (!CompileTexture(filepath, texture_index, texture_id, mipmap, flip))
                {
                    CIRI_WARN("Failed to compile texture: {}", filepath);
                }
            }
            else
            {
                *texture_id = m_TextureList[filepath];
            }
        }
        else
        {
            *texture_id = m_DefaultTextureID;
        }
    }

    bool MaterialLibrary::CompileTexture(std::string filepath, uint32_t texture_index, uint32_t *texture_id, bool mipmap, bool flip)
    {
        int32_t w, h;
        int32_t nrChannels;

        stbi_set_flip_vertically_on_load(flip);

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
        if (mipmap)
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
            if (mipmap)
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            if (mipmap)
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