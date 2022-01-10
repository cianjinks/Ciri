#include "Shader.h"

#include <glad/glad.h>

namespace Ciri
{
    ShaderLibrary::ShaderLibrary()
    {
        RegisterShaders();
        CompileShaders();
    }

    ShaderLibrary::~ShaderLibrary()
    {
        for (auto &pair : m_ShaderList)
        {
            Shader *shader = pair.second;
            delete shader;
        }
    }

    void ShaderLibrary::BindShader(ShaderType type)
    {
        Shader *shader = m_ShaderList[type];
        glUseProgram(shader->program_id);
        m_CurrentShaderType = type;
    }

    void ShaderLibrary::RegisterShaders()
    {
        // NONE
        Shader *none = new Shader(ShaderType::NONE, "", "");
        none->program_id = 0;
        m_ShaderList[ShaderType::NONE] = none;

        AddShader(ShaderType::DIFFUSE, "resources/shader/base.vert", "resources/shader/diffuse.frag");
        AddShader(ShaderType::FLAT_NORMAL, "resources/shader/base.vert", "resources/shader/flat_normal.frag");
        AddShader(ShaderType::SMOOTH_NORMAL, "resources/shader/base.vert", "resources/shader/normal.frag");
        AddShader(ShaderType::TEXTURE, "resources/shader/base.vert", "resources/shader/texture.frag");
    }

    void ShaderLibrary::CompileShaders()
    {
        uint32_t vert_object, frag_object;
        for (auto &pair : m_ShaderList)
        {
            Shader *shader = pair.second;
            if (m_ObjectMap.find(shader->vert_src) == m_ObjectMap.end())
            {
                std::string vert_code = m_SourceMap[shader->vert_src];
                const char *vert_raw = vert_code.c_str();
                vert_object = glCreateShader(GL_VERTEX_SHADER);

                glShaderSource(vert_object, 1, &vert_raw, NULL);
                glCompileShader(vert_object);

                if (CheckCompilation(vert_object))
                {
                    m_ObjectMap[shader->vert_src] = vert_object;
                }
            }
            else
            {
                vert_object = m_ObjectMap[shader->vert_src];
            }

            if (m_ObjectMap.find(shader->frag_src) == m_ObjectMap.end())
            {
                std::string frag_code = m_SourceMap[shader->frag_src];
                const char *frag_raw = frag_code.c_str();
                frag_object = glCreateShader(GL_VERTEX_SHADER);

                glShaderSource(frag_object, 1, &frag_raw, NULL);
                glCompileShader(frag_object);

                if (CheckCompilation(frag_object))
                {
                    m_ObjectMap[shader->frag_src] = frag_object;
                }
            }
            else
            {
                frag_object = m_ObjectMap[shader->frag_src];
            }

            shader->program_id = glCreateProgram();
            glAttachShader(shader->program_id, vert_object);
            glAttachShader(shader->program_id, frag_object);
            glLinkProgram(shader->program_id);
            glValidateProgram(shader->program_id);
            glUseProgram(0); // Is this necessary?
        }
    }

    void ShaderLibrary::AddShader(ShaderType type, const char *vert_src, const char *frag_src)
    {
        if (m_SourceMap.find(vert_src) == m_SourceMap.end())
        {
            std::string vert_code = ParseFile(vert_src);
            if (vert_code == "")
            {
                std::cout << "Failed to vertex shader from file: " << vert_src << std::endl;
            }
            m_SourceMap[vert_src] = vert_code;
        }

        if (m_SourceMap.find(frag_src) == m_SourceMap.end())
        {
            std::string frag_code = ParseFile(frag_src);
            if (frag_code == "")
            {
                std::cout << "Failed to parse fragment shader from file: " << frag_src << std::endl;
            }
            m_SourceMap[frag_src] = frag_code;
        }

        Shader *shader = new Shader(type, vert_src, frag_src);
        m_ShaderList[type] = shader;
    }

    std::string ShaderLibrary::ParseFile(const char *file_path)
    {
        std::string data = "";
        std::ifstream in(file_path, std::ios::in);
        if (in)
        {
            in.seekg(0, std::ios::end);
            data.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&data[0], data.size());
            in.close();
        }
        return data;
    }

    bool ShaderLibrary::CheckCompilation(uint32_t shader_object)
    {
        int32_t result;
        glGetShaderiv(shader_object, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            std::cout << "Shader " << shader_object << " failed:" << std::endl;
            int32_t length;
            glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &length);
            char *log = new char[length];
            glGetShaderInfoLog(shader_object, length, &length, log);
            std::cout << log << std::endl;
            delete[] log;
            glDeleteShader(shader_object);
        }
        return result;
    }
}