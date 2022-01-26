#include "Mesh.h"

#include <glad/glad.h>

namespace Ciri
{
	Mesh::Mesh()
	{
	}

	Mesh::Mesh(std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec2> texcoord_data)
		: m_PositionData(position_data), m_NormalData(normal_data), m_TexCoordData(texcoord_data)
	{
	}

	void Mesh::Construct()
	{
		if (!m_VAO)
		{
			glGenVertexArrays(1, &m_VAO);
			glGenBuffers(1, &m_VBO);
		}

		std::vector<float> data;
		data.reserve((m_PositionData.size() * 3) + (m_NormalData.size() * 3) + (m_TexCoordData.size() * 2));
		for (int i = 0; i < m_PositionData.size(); i++)
		{
			data.push_back(m_PositionData[i].x);
			data.push_back(m_PositionData[i].y);
			data.push_back(m_PositionData[i].z);
			data.push_back(m_NormalData[i].x);
			data.push_back(m_NormalData[i].y);
			data.push_back(m_NormalData[i].z);
			data.push_back(m_TexCoordData[i].x);
			data.push_back(m_TexCoordData[i].y);
		}

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		TriCount = (int32_t)m_PositionData.size();
	}
}