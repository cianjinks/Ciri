#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

namespace Ciri
{

    enum class ShaderType
    {
        NONE = 0,
        DIFFUSE,
        FLAT_NORMAL,
        SMOOTH_NORMAL,
        TEXTURE
    };

    struct Shader
    {
        ShaderType type;
        const char *vert_src, *frag_src;
        uint32_t program_id;

        Shader(ShaderType type, const char *vert_src, const char *frag_src)
            : type(type), vert_src(vert_src), frag_src(frag_src) {}
    };

    class ShaderLibrary
    {
    private:
        std::unordered_map<ShaderType, Shader *> m_ShaderList;
        std::unordered_map<const char *, std::string> m_SourceMap; // File Path -> Source Code
        std::unordered_map<const char *, uint32_t> m_ObjectMap;    // File Path -> Shader Object

        ShaderType m_CurrentShaderType = ShaderType::NONE;

    public:
        ShaderLibrary();
        ~ShaderLibrary();

        void BindShader(ShaderType type);
        ShaderType GetShaderType() { return m_CurrentShaderType; }
        Shader *GetShader() { return m_ShaderList[m_CurrentShaderType]; }

    private:
        void RegisterShaders();
        void CompileShaders();
        void AddShader(ShaderType type, const char *vert_src, const char *frag_src);

        std::string ParseFile(const char *file_path);
        bool CheckCompilation(uint32_t shader_object);
    };

}