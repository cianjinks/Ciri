#ifndef CIRI_MATERIAL_H
#define CIRI_MATERIAL_H

#include <glm/glm.hpp>

namespace Ciri
{

    struct MaterialSpecification
    {
        std::string name = "null";
        bool mipmap = false;
        bool flip = false;

        /* TODO: Create Texture2D abstraction. */
        std::string baseColorFilepath;
        std::string normalFilepath;
        std::string metallicRoughnessFilepath;
        std::string occlusionFilepath;
        std::string emissiveFilepath;

        // PBR textures (https://learnopengl.com/PBR/Theory, https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html)
        uint32_t baseColorTextureID = 0; // diffuse, albedo
        uint32_t normalTextureID = 0;
        uint32_t metallicRoughnessTextureID = 0;
        uint32_t occlusionTextureID = 0; // ambient occlusion
        uint32_t emissiveTextureID = 0;

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
    };

    class Material
    {
    public:
        MaterialSpecification spec;

    private:
        Material(MaterialSpecification spec) : spec(spec){};
        static S<Material> Create(MaterialSpecification spec);

        friend class MaterialLibrary;
    };

    class MaterialLibrary
    {
    private:
        std::map<std::string, S<Material>> m_MaterialList; // name -> material
        std::map<std::string, uint32_t> m_TextureList;     // filepath -> texture id

        uint32_t m_DefaultTextureID; /* TODO: Replace with Texture2D. */
        S<Material> m_DefaultMaterial;

    public:
        MaterialLibrary();
        ~MaterialLibrary() = default;

        S<Material> GetMaterial(std::string name);
        S<Material> GetDefaultMaterial() { return m_DefaultMaterial; }
        const std::map<std::string, S<Material>> &GetMaterials() const { return m_MaterialList; };
        const std::map<std::string, uint32_t> &GetTextures() const { return m_TextureList; };

        S<Material> CreateMaterial(MaterialSpecification &spec);

    private:
        // Add new texture to `m_TextureList`, calls `CompileTexture`
        void RegisterTexture(std::string filepath, uint32_t texture_index, uint32_t *texture_id, bool mipmap, bool flip);
        // Load texture from filepath and upload to GPU, assigning `texture_id` with active index `texture_index`
        bool CompileTexture(std::string filepath, uint32_t texture_index, uint32_t *texture_id, bool mipmap, bool flip);

        void CreateDefaultTexture();
        void CreateDefaultMaterial();
    };
}

#endif