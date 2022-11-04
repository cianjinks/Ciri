#ifndef CIRI_MESH_H
#define CIRI_MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ciri
{
    class Mesh
    {
    public:
        int32_t TriCount = 0;
        // Public for rendering tests
        uint32_t m_VAO = 0;

        static const glm::vec3 s_DefaultColor; // For primitives
        bool IsIndexed = false;

    private:
        uint32_t m_VBO = 0;
        uint32_t m_IBO = 0;

    protected:
        /* Note: Careful not to change data types here. Their size is used in `construct` and in importers. Need to abstract data formats. */
        std::vector<glm::vec3> m_PositionData;
        std::vector<glm::vec3> m_NormalData;
        std::vector<glm::vec2> m_TexCoordData;
        std::vector<glm::i32vec4> m_BoneIDData;
        std::vector<glm::vec4> m_BoneWeightData;
        std::vector<uint16_t> m_IndexData;

    public:
        Mesh();
        Mesh(std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec2> texcoord_data);
        Mesh(std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec2> texcoord_data, std::vector<uint16_t> index_data);
        Mesh(std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec2> texcoord_data, std::vector<uint16_t> index_data, std::vector<glm::i32vec4> boneid_data, std::vector<glm::vec4> boneweight_data);

        ~Mesh() = default;

        // Upload mesh data to GPU (generate VAO, VBO)
        void Construct();

        int32_t GetIndexCount()
        {
            if (IsIndexed)
            {
                return m_IndexData.size();
            }
            else
            {
                return -1;
            }
        }
    };
}

#endif