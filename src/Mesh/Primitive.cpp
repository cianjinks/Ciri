#include "Primitive.h"

#include "Util/Util.h"

namespace Ciri
{
    Cube::Cube()
    {
        // clang-format off
        m_PositionData = {
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f,  1.0f, -1.0f),
            glm::vec3( 1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f,  1.0f,  1.0f),
            glm::vec3( 1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),

            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),

            glm::vec3( 1.0f,  1.0f,  1.0f),
            glm::vec3( 1.0f,  1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f,  1.0f,  1.0f),

            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),

            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3( 1.0f,  1.0f, -1.0f),
            glm::vec3( 1.0f,  1.0f,  1.0f),
            glm::vec3( 1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
        };

        m_NormalData = {
            glm::vec3( 0.0f,  0.0f, -1.0f),
            glm::vec3( 0.0f,  0.0f, -1.0f),
            glm::vec3( 0.0f,  0.0f, -1.0f),
            glm::vec3( 0.0f,  0.0f, -1.0f),
            glm::vec3( 0.0f,  0.0f, -1.0f),
            glm::vec3( 0.0f,  0.0f, -1.0f),

            glm::vec3( 0.0f,  0.0f,  1.0f),
            glm::vec3( 0.0f,  0.0f,  1.0f),
            glm::vec3( 0.0f,  0.0f,  1.0f),
            glm::vec3( 0.0f,  0.0f,  1.0f),
            glm::vec3( 0.0f,  0.0f,  1.0f),
            glm::vec3( 0.0f,  0.0f,  1.0f),

            glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-1.0f,  0.0f,  0.0f),
            glm::vec3(-1.0f,  0.0f,  0.0f),

            glm::vec3( 1.0f,  0.0f,  0.0f),
            glm::vec3( 1.0f,  0.0f,  0.0f),
            glm::vec3( 1.0f,  0.0f,  0.0f),
            glm::vec3( 1.0f,  0.0f,  0.0f),
            glm::vec3( 1.0f,  0.0f,  0.0f),
            glm::vec3( 1.0f,  0.0f,  0.0f),

            glm::vec3( 0.0f, -1.0f,  0.0f),
            glm::vec3( 0.0f, -1.0f,  0.0f),
            glm::vec3( 0.0f, -1.0f,  0.0f),
            glm::vec3( 0.0f, -1.0f,  0.0f),
            glm::vec3( 0.0f, -1.0f,  0.0f),
            glm::vec3( 0.0f, -1.0f,  0.0f),

            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f)
        };

        m_TexCoordData = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),

            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),

            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),

            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),

            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f),

            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f)
        };

        // clang-format on
    }

    Quad::Quad()
    {
        // clang-format off
        m_PositionData = {
            glm::vec3( 1.0f, 0.0f, -1.0f),
            glm::vec3( 1.0f, 0.0f,  1.0f),
            glm::vec3(-1.0f, 0.0f,  1.0f),
            glm::vec3( 1.0f, 0.0f, -1.0f),
            glm::vec3(-1.0f, 0.0f, -1.0f),
            glm::vec3(-1.0f, 0.0f,  1.0f)

        };

        m_NormalData = {
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f),
            glm::vec3( 0.0f,  1.0f,  0.0f)
        };

        m_TexCoordData = {
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f)
        };
        // clang-format on
    }

    // UV sphere
    Sphere::Sphere(uint32_t h_segments, uint32_t v_segments, bool flat)
    {
        for (uint32_t v = 0; v < v_segments; v++)
        {
            float theta1 = glm::pi<float>() * (float(v) / float(v_segments));
            float theta2 = glm::pi<float>() * (float(v + 1) / float(v_segments));

            for (uint32_t h = 0; h < h_segments; h++)
            {
                float phi1 = glm::two_pi<float>() * (float(h) / float(h_segments));
                float phi2 = glm::two_pi<float>() * (float(h + 1) / float(h_segments));

                glm::vec3 v1 = Util::spherical_to_cartesian(1.0f, theta1, phi1);
                glm::vec3 v2 = Util::spherical_to_cartesian(1.0f, theta1, phi2);
                glm::vec3 v3 = Util::spherical_to_cartesian(1.0f, theta2, phi2);
                glm::vec3 v4 = Util::spherical_to_cartesian(1.0f, theta2, phi1);

                m_PositionData.push_back(v1);
                m_PositionData.push_back(v2);
                m_PositionData.push_back(v3);

                m_PositionData.push_back(v3);
                m_PositionData.push_back(v4);
                m_PositionData.push_back(v1);

                // Shade as flat quads
                if (flat)
                {
                    glm::vec3 n2 = Util::calc_tri_surface_normal(v1, v2, v3);
                    m_NormalData.push_back(glm::normalize(n2));
                    m_NormalData.push_back(glm::normalize(n2));
                    m_NormalData.push_back(glm::normalize(n2));

                    glm::vec3 n1 = Util::calc_tri_surface_normal(v3, v4, v1);
                    m_NormalData.push_back(glm::normalize(n1));
                    m_NormalData.push_back(glm::normalize(n1));
                    m_NormalData.push_back(glm::normalize(n1));
                }
                else
                {
                    m_NormalData.push_back(glm::normalize(v1));
                    m_NormalData.push_back(glm::normalize(v2));
                    m_NormalData.push_back(glm::normalize(v3));

                    m_NormalData.push_back(glm::normalize(v3));
                    m_NormalData.push_back(glm::normalize(v4));
                    m_NormalData.push_back(glm::normalize(v1));
                }

                glm::vec2 t1 = glm::vec2(float(v) / float(v_segments), float(h) / float(h_segments));
                glm::vec2 t2 = glm::vec2(float(v) / float(v_segments), float(h + 1) / float(h_segments));
                glm::vec2 t3 = glm::vec2(float(v + 1) / float(v_segments), float(h + 1) / float(h_segments));
                glm::vec2 t4 = glm::vec2(float(v + 1) / float(v_segments), float(h) / float(h_segments));

                m_TexCoordData.push_back(t1);
                m_TexCoordData.push_back(t2);
                m_TexCoordData.push_back(t3);

                m_TexCoordData.push_back(t3);
                m_TexCoordData.push_back(t4);
                m_TexCoordData.push_back(t1);
            }
        }
    }
}