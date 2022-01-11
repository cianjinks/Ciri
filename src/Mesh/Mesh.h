#ifndef CIRI_MESH_H
#define CIRI_MESH_H

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace Ciri
{
    class Mesh
    {
    public:
        const char *Name;
        int32_t TriCount;

    private:
        uint32_t m_VAO;
        uint32_t m_VBO;

        std::vector<glm::vec3> m_PositionData;
        std::vector<glm::vec3> m_NormalData;
        std::vector<glm::vec3> m_ColorData;
        std::vector<glm::vec2> m_TexCoordData;

    public:
        Mesh(const char *name, std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec3> color_data, std::vector<glm::vec2> texcoord_data);
        ~Mesh() = default;

        // Upload mesh data to GPU (generate VAO, VBO)
        void Construct();
    };
}

#endif