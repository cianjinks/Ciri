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

    void ShaderLibrary::SetMat4f(const char *name, float *mat4)
    {
        uint32_t loc = glGetUniformLocation(m_CurrentShader->program_id, name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, mat4);
    }

    void ShaderLibrary::SetInt1i(const char *name, int32_t int1)
    {
        uint32_t loc = glGetUniformLocation(m_CurrentShader->program_id, name);
        glUniform1i(loc, int1);
    }

    void ShaderLibrary::SetFloat1f(const char *name, float float1)
    {
        uint32_t loc = glGetUniformLocation(m_CurrentShader->program_id, name);
        glUniform1f(loc, float1);
    }

    void ShaderLibrary::SetVec3f(const char *name, glm::vec3 vec3)
    {
        uint32_t loc = glGetUniformLocation(m_CurrentShader->program_id, name);
        glUniform3fv(loc, 1, &vec3.x);
    }

    void ShaderLibrary::BindShader(ShaderType type)
    {
        Shader *shader = m_ShaderList[type];
        glUseProgram(shader->program_id);
        m_CurrentShaderType = type;
        m_CurrentShader = shader;
    }

    void ShaderLibrary::RegisterShaders()
    {
        // NONE
        Shader *none = new Shader("None", ShaderType::NONE, "", "");
        none->program_id = 0;
        m_ShaderList[ShaderType::NONE] = none;
        m_ShaderCount++;
        m_CurrentShaderType = ShaderType::NONE;
        m_CurrentShader = none;

        AddShader("Geometry Buffer", ShaderType::GEOMETRY_BUFFER, "resources/shader/gbuffer.vert", "resources/shader/gbuffer.frag");
        AddShader("Geometry Buffer Anim", ShaderType::GEOMETRY_BUFFER_ANIM, "resources/shader/gbuffer_anim.vert", "resources/shader/gbuffer.frag");

        AddShader("Base Color", ShaderType::BASE_COLOR, "resources/shader/screen_quad.vert", "resources/shader/base_color.frag");
        AddShader("Linear Depth", ShaderType::LINEAR_DEPTH, "resources/shader/screen_quad.vert", "resources/shader/linear_depth.frag");
        AddShader("Normal", ShaderType::NORMAL, "resources/shader/screen_quad.vert", "resources/shader/normal.frag");
        AddShader("Occlusion", ShaderType::OCCLUSION, "resources/shader/screen_quad.vert", "resources/shader/occlusion.frag");
        AddShader("Metallic & Roughness", ShaderType::METALLIC_ROUGHNESS, "resources/shader/screen_quad.vert", "resources/shader/metallic_roughness.frag");
        AddShader("Emissive", ShaderType::EMISSIVE, "resources/shader/screen_quad.vert", "resources/shader/emissive.frag");

        AddShader("World Position", ShaderType::WORLD_POSITION, "resources/shader/screen_quad.vert", "resources/shader/world_position.frag");
        AddShader("Origin Distance", ShaderType::ORIGIN_DISTANCE, "resources/shader/screen_quad.vert", "resources/shader/dist_origin.frag");
    }

    void ShaderLibrary::CompileShaders()
    {
        uint32_t vert_object, frag_object;
        for (auto &pair : m_ShaderList)
        {
            Shader *shader = pair.second;

            if (shader->type == ShaderType::NONE)
            {
                continue;
            }

            if (m_ObjectMap.find(shader->vert_src) == m_ObjectMap.end())
            {
                std::string vert_code = m_SourceMap[shader->vert_src];
                const char *vert_raw = vert_code.c_str();
                vert_object = glCreateShader(GL_VERTEX_SHADER);

                glShaderSource(vert_object, 1, &vert_raw, NULL);
                glCompileShader(vert_object);

                if (CheckCompilation(shader->vert_src, vert_object))
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
                frag_object = glCreateShader(GL_FRAGMENT_SHADER);

                glShaderSource(frag_object, 1, &frag_raw, NULL);
                glCompileShader(frag_object);

                if (CheckCompilation(shader->frag_src, frag_object))
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

    void ShaderLibrary::AddShader(const char *name, ShaderType type, const char *vert_src, const char *frag_src)
    {
        if (m_SourceMap.find(vert_src) == m_SourceMap.end())
        {
            std::string vert_code = ParseFile(vert_src);
            if (vert_code == "")
            {
                CIRI_ERROR("Failed to parse vertex shader from file: {}", vert_src);
            }
            m_SourceMap[vert_src] = vert_code;
        }

        if (m_SourceMap.find(frag_src) == m_SourceMap.end())
        {
            std::string frag_code = ParseFile(frag_src);
            if (frag_code == "")
            {
                CIRI_ERROR("Failed to parse fragment shader from file: {}", frag_src);
            }
            m_SourceMap[frag_src] = frag_code;
        }

        Shader *shader = new Shader(name, type, vert_src, frag_src);
        m_ShaderList[type] = shader;
        m_ShaderCount++;
    }

    std::string ShaderLibrary::ParseFile(const char *file_path)
    {
        std::string data = "";
        std::ifstream in(file_path, std::ios::in);
        if (in)
        {
            in.seekg(0, std::ios::end);
            data.resize((const uint32_t)in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&data[0], data.size());
            in.close();
        }
        return data;
    }

    bool ShaderLibrary::CheckCompilation(const char *name, uint32_t shader_object)
    {
        int32_t result;
        glGetShaderiv(shader_object, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            CIRI_ERROR("Shader {} failed:", name);
            int32_t length;
            glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &length);
            char *log = new char[length];
            glGetShaderInfoLog(shader_object, length, &length, log);
            CIRI_ERROR(log);
            delete[] log;
            glDeleteShader(shader_object);
        }
        return result;
    }
}