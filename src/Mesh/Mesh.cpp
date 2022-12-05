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

	Mesh::Mesh(std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec2> texcoord_data, std::vector<uint16_t> index_data)
		: m_PositionData(position_data), m_NormalData(normal_data), m_TexCoordData(texcoord_data), m_IndexData(index_data), IsIndexed(index_data.size() > 0)
	{
	}

	Mesh::Mesh(std::vector<glm::vec3> position_data, std::vector<glm::vec3> normal_data, std::vector<glm::vec2> texcoord_data, std::vector<uint16_t> index_data, std::vector<glm::i32vec4> boneid_data, std::vector<glm::vec4> boneweight_data)
		: m_PositionData(position_data), m_NormalData(normal_data), m_TexCoordData(texcoord_data), m_IndexData(index_data), IsIndexed(index_data.size() > 0), m_BoneIDData(boneid_data), m_BoneWeightData(boneweight_data)
	{
	}

	void Mesh::Construct()
	{
		if (!m_VAO)
		{
			glGenVertexArrays(1, &m_VAO);
			glGenBuffers(1, &m_VBO);
			if (IsIndexed)
			{
				glGenBuffers(1, &m_IBO);
			}
		}

		/* Bone data is required for now. Should be fixed with a mesh data format abstraction. */
		if (m_BoneIDData.empty()) { m_BoneIDData.resize(m_PositionData.size()); }
		if (m_BoneWeightData.empty()) { m_BoneWeightData.resize(m_PositionData.size()); }

		std::vector<uint8_t> data;
		data.reserve(
				(m_PositionData.size() * sizeof(glm::vec3))
				 + (m_NormalData.size() * sizeof(glm::vec3))
				 + (m_TexCoordData.size() * sizeof(glm::vec2))
				 + (m_BoneWeightData.size() * sizeof(glm::vec4))
				 + (m_BoneIDData.size() * sizeof(glm::i32vec4)));

		for (size_t i = 0; i < m_PositionData.size(); i++)
		{
			/* TODO: Don't interleave data. Will likely make this much faster. */
			uint8_t* position = reinterpret_cast<uint8_t*>(glm::value_ptr(m_PositionData[i]));
			for (int b = 0; b < sizeof(glm::vec3); b++) { data.push_back(position[b]); }
			uint8_t* normal = reinterpret_cast<uint8_t*>(glm::value_ptr(m_NormalData[i]));
			for (int b = 0; b < sizeof(glm::vec3); b++) { data.push_back(normal[b]); }
			uint8_t* texcoord = reinterpret_cast<uint8_t*>(glm::value_ptr(m_TexCoordData[i]));
			for (int b = 0; b < sizeof(glm::vec2); b++) { data.push_back(texcoord[b]); }
			uint8_t* boneid = reinterpret_cast<uint8_t*>(glm::value_ptr(m_BoneIDData[i]));
			for (int b = 0; b < sizeof(glm::i32vec4); b++) { data.push_back(boneid[b]); }
			uint8_t* boneweight = reinterpret_cast<uint8_t*>(glm::value_ptr(m_BoneWeightData[i]));
			for (int b = 0; b < sizeof(glm::vec4); b++) { data.push_back(boneweight[b]); }
		}

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(uint8_t), data.data(), GL_STATIC_DRAW);
		if (IsIndexed)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexData.size() * sizeof(uint16_t), m_IndexData.data(), GL_STATIC_DRAW);
		}

		GLsizei vertex_size = (12 * sizeof(float)) + (4 * sizeof(uint32_t));
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribIPointer(3, 4, GL_INT, vertex_size, (void *)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, vertex_size, (void *)((8 * sizeof(float)) + (4 * sizeof(uint32_t))));
		glEnableVertexAttribArray(4);

		TriCount = (int32_t)(m_PositionData.size() / 3);
	}
}