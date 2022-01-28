#ifndef CIRI_SHADER_H
#define CIRI_SHADER_H

#include <glm/glm.hpp>

namespace Ciri
{

    enum class ShaderType
    {
        NONE = 0,
        FLAT_NORMAL,
        SMOOTH_NORMAL,
        ALBEDO,

        // Raw PBR Textures
        ALBEDO_TEXTURE,
        NORMAL_TEXTURE,
        METALLIC_TEXTURE,
        ROUGHNESS_TEXTURE,
        EMISSIVE_TEXTURE,
        OCCLUSION_TEXTURE

    };

    struct Shader
    {
        const char *name;
        ShaderType type;
        const char *vert_src, *frag_src;
        uint32_t program_id;

        Shader(const char *name, ShaderType type, const char *vert_src, const char *frag_src)
            : name(name), type(type), vert_src(vert_src), frag_src(frag_src) {}
    };

    class ShaderLibrary
    {
    private:
        std::map<ShaderType, Shader *> m_ShaderList;
        std::unordered_map<const char *, std::string> m_SourceMap; // File Path -> Source Code
        std::unordered_map<const char *, uint32_t> m_ObjectMap;    // File Path -> Shader Object

        ShaderType m_CurrentShaderType = ShaderType::NONE;
        Shader *m_CurrentShader;

    public:
        ShaderLibrary();
        ~ShaderLibrary();

        void BindShader(ShaderType type);
        ShaderType GetShaderType() { return m_CurrentShaderType; }
        Shader *GetShader() { return m_CurrentShader; }
        std::map<ShaderType, Shader *> &GetShaderList() { return m_ShaderList; }

        // Uniforms
        void SetMat4f(const char *name, float *mat4);
        void SetInt1i(const char *name, int32_t int1);
        void SetVec3f(const char *name, glm::vec3 vec3);

    private:
        void RegisterShaders();
        void CompileShaders();
        void AddShader(const char *name, ShaderType type, const char *vert_src, const char *frag_src);

        std::string ParseFile(const char *file_path);
        bool CheckCompilation(uint32_t shader_object);
    };

}

#endif