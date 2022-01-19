#ifndef CIRI_MATERIAL_H
#define CIRI_MATERIAL_H

#include <cstdint>
#include <map>
#include <unordered_map>
#include <iostream>

#include <glm/glm.hpp>

namespace Ciri
{

    struct MaterialSettings
    {
        bool mipmap = false;
    };

    class Material
    {
    public:
        // https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
        glm::vec3 baseColor = glm::vec3(0.0f);
        float subsurface = 0.0f;
        float metallic = 0.0f;
        float specular = 0.0f;
        float specularTint = 0.0f;
        float roughness = 0.0f;
        float anisotropic = 0.0f;
        float sheen = 0.0f;
        float sheenTint = 0.0f;
        float clearcoat = 0.0f;
        float clearcoatGloss = 0.0f;

        // PBR textures (https://learnopengl.com/PBR/Theory, https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html)
        uint32_t baseColorTextureID = 0; // diffuse, albedo
        uint32_t normalTextureID = 0;
        uint32_t metallicTextureID = 0;
        uint32_t roughnessTextureID = 0;
        uint32_t emissiveTextureID = 0;
        uint32_t occlusionTextureID = 0; // ambient occlusion

        // Settings
        MaterialSettings settings;

    public:
        Material();
    };

    class MaterialLibrary
    {
    private:
        std::map<const char *, Material *> m_MaterialList;        // name -> material
        std::unordered_map<const char *, uint32_t> m_TextureList; // filepath -> texture id

    public:
        MaterialLibrary();
        ~MaterialLibrary() = default;

        Material *GetMaterial(const char *name);

        Material *CreateMaterial(const char *name, glm::vec3 base_color,
                                 MaterialSettings settings,
                                 const char *baseColorTexture = nullptr,
                                 const char *normalTexture = nullptr,
                                 const char *metallicTexture = nullptr,
                                 const char *roughnessTexture = nullptr,
                                 const char *emissiveTexture = nullptr,
                                 const char *occlusionTexture = nullptr,
                                 float subsurface = 0.0f,
                                 float metallic = 0.0f,
                                 float specular = 0.0f,
                                 float specularTint = 0.0f,
                                 float roughness = 0.0f,
                                 float anisotropic = 0.0f,
                                 float sheen = 0.0f,
                                 float sheenTint = 0.0f,
                                 float clearcoat = 0.0f,
                                 float clearcoatGloss = 0.0f);

    private:
        // Add new texture to `m_TextureList`, calls `CompileTexture`
        void RegisterTexture(const char *filepath, uint32_t texture_index, uint32_t *texture_id, MaterialSettings settings);
        // Load texture from filepath and upload to GPU, assigning `texture_id` with active index `texture_index`
        bool CompileTexture(const char *filepath, uint32_t texture_index, uint32_t *texture_id, MaterialSettings settings);
    };
}

#endif