#ifndef CIRI_MATERIAL_H
#define CIRI_MATERIAL_H

#include <cstdint>

#include <glm/glm.hpp>

namespace Ciri
{

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
        uint32_t baseColorTextureID; // diffuse, albedo
        uint32_t normalTextureID;
        uint32_t metallicTextureID;
        uint32_t roughnessTextureID;
        uint32_t emissiveTextureID;
        uint32_t occlusionTextureID; // ambient occlusion

    public:
        Material();
    };

    class MaterialLibrary
    {
    public:
        MaterialLibrary();
    };
}

#endif