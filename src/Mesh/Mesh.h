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
        int32_t TriCount = 0;
        // Public for rendering tests
        uint32_t m_VAO = 0;

    private:
        uint32_t m_VBO = 0;

    protected:
        std::vector<glm::vec3> m_PositionData;
        std::vector<glm::vec3> m_NormalData;
        std::vector<glm::vec3> m_ColorData;
        std::vector<glm::vec2> m_TexCoordData;

        static const glm::vec3 s_DefaultColor; // For primitives

    public:
        Mesh();
        Mesh(std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec3> color_data, std::vector<glm::vec2> texcoord_data);
        ~Mesh() = default;

        // Upload mesh data to GPU (generate VAO, VBO)
        void Construct();
    };
}

#endif