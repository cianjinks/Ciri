#include "Math.h"

namespace Ciri
{
    namespace Math
    {
        Transform::Transform(const glm::mat4 &mat)
        {
            DecomposeTransform(mat, m_LocalTranslation, m_LocalRotation, m_LocalScale);
            m_LocalMatrix = mat;
            m_WorldMatrix = mat;
        }

        void Transform::Update()
        {
            m_LocalMatrix = ComposeTransform(m_LocalTranslation, m_LocalRotation, m_LocalScale);
            m_WorldMatrix = m_ParentMatrix * m_LocalMatrix;
            m_Dirty = false;
        }

        void Transform::SetParentMatrix(const glm::mat4 &mat)
        {
            if (m_Dirty)
            {
                Update();
            }

            m_ParentMatrix = mat;
            m_WorldMatrix = m_ParentMatrix * m_LocalMatrix;
        }

        void Transform::SetLocalMatrix(const glm::mat4 &mat)
        {
            DecomposeTransform(mat, m_LocalTranslation, m_LocalRotation, m_LocalScale);
            m_LocalMatrix = mat;
            m_WorldMatrix = m_ParentMatrix * m_LocalMatrix;
        }

        void Transform::SetLocalTranslation(const glm::vec3 &translation)
        {
            m_LocalTranslation = translation;
            m_Dirty = true;
        }

        void Transform::SetLocalRotation(const glm::vec3 &rotation)
        {
            m_LocalRotation = rotation;
            m_Dirty = true;
        }

        void Transform::SetLocalScale(const glm::vec3 &scale)
        {
            m_LocalScale = scale;
            m_Dirty = true;
        }

        const glm::mat4 &Transform::GetLocalMatrix()
        {
            if (m_Dirty)
            {
                Update();
            }

            return m_LocalMatrix;
        }

        const glm::mat4 &Transform::GetWorldMatrix()
        {
            if (m_Dirty)
            {
                Update();
            }

            return m_WorldMatrix;
        }

        const glm::vec3 Transform::GetWorldTranslation() const
        {
            glm::vec3 translation, rotation, scale;
            DecomposeTransform(m_WorldMatrix, translation, rotation, scale);
            return translation;
        }

        const glm::vec3 Transform::GetWorldRotation() const
        {
            glm::vec3 translation, rotation, scale;
            DecomposeTransform(m_WorldMatrix, translation, rotation, scale);
            return rotation;
        }

        glm::mat4 ComposeTransform(const glm::vec3 &translation, const glm::quat &rotation, const glm::vec3 &scale)
        {
            glm::mat4 result = glm::mat4(1.0f);
            result = glm::translate(result, translation);
            result *= glm::toMat4(rotation);
            result = glm::scale(result, scale);
            return result;
        }

        glm::mat4 ComposeTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
        {
            glm::mat4 result = glm::mat4(1.0f);
            result = glm::translate(result, translation);
            result *= glm::toMat4(glm::quat(rotation));
            result = glm::scale(result, scale);
            return result;
        }

        /* Taken from https://github.com/TheCherno/Hazel which modifies glm::decompose. */
        bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &r_translation, glm::vec3 &r_rotation, glm::vec3 &r_scale)
        {
            using namespace glm;
            using T = float;

            mat4 LocalMatrix(transform);

            // Normalize the matrix.
            if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
                return false;

            // First, isolate perspective.  This is the messiest.
            if (
                epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
                epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
                epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
            {
                // Clear the perspective partition
                LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
                LocalMatrix[3][3] = static_cast<T>(1);
            }

            // Next take care of translation (easy).
            r_translation = vec3(LocalMatrix[3]);
            LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

            vec3 Row[3], Pdum3;

            // Now get scale and shear.
            for (length_t i = 0; i < 3; ++i)
                for (length_t j = 0; j < 3; ++j)
                    Row[i][j] = LocalMatrix[i][j];

            // Compute X scale factor and normalize first row.
            r_scale.x = length(Row[0]);
            Row[0] = detail::scale(Row[0], static_cast<T>(1));
            r_scale.y = length(Row[1]);
            Row[1] = detail::scale(Row[1], static_cast<T>(1));
            r_scale.z = length(Row[2]);
            Row[2] = detail::scale(Row[2], static_cast<T>(1));

            // At this point, the matrix (in rows[]) is orthonormal.
            // Check for a coordinate system flip.  If the determinant
            // is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

            r_rotation.y = asin(-Row[0][2]);
            if (cos(r_rotation.y) != 0)
            {
                r_rotation.x = atan2(Row[1][2], Row[2][2]);
                r_rotation.z = atan2(Row[0][1], Row[0][0]);
            }
            else
            {
                r_rotation.x = atan2(-Row[2][0], Row[1][1]);
                r_rotation.z = 0;
            }

            return true;
        }

        bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &r_translation, glm::quat &r_rotation, glm::vec3 &r_scale)
        {
            glm::vec3 rotation;
            if (DecomposeTransform(transform, r_translation, rotation, r_scale))
            {
                r_rotation = glm::quat(rotation);
                return true;
            }
            return false;
        }
    }
}