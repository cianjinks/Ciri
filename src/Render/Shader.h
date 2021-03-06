#ifndef CIRI_SHADER_H
#define CIRI_SHADER_H

#include <glm/glm.hpp>

namespace Ciri
{

    enum class ShaderType
    {
        NONE = 0,
        GEOMETRY_BUFFER,

        BASE_COLOR,
        LINEAR_DEPTH,
        NORMAL,
        OCCLUSION,
        METALLIC_ROUGHNESS,
        EMISSIVE,

        WORLD_POSITION,
        ORIGIN_DISTANCE
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
        uint32_t m_ShaderCount = 0;
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
        uint32_t GetShaderCount() { return m_ShaderCount; }
        std::map<ShaderType, Shader *> &GetShaderList() { return m_ShaderList; }

        // Uniforms
        void SetMat4f(const char *name, float *mat4);
        void SetInt1i(const char *name, int32_t int1);
        void SetFloat1f(const char *name, float float1);
        void SetVec3f(const char *name, glm::vec3 vec3);

    private:
        void RegisterShaders();
        void CompileShaders();
        void AddShader(const char *name, ShaderType type, const char *vert_src, const char *frag_src);

        std::string ParseFile(const char *file_path);
        bool CheckCompilation(const char *name, uint32_t shader_object);
    };

}

#endif