#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Isle
{
    class Transform
    {
    public:
        glm::vec3 m_Translation;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;
    public:
        Transform()
        {
            m_Translation = glm::vec3(0.0f);
            m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            m_Scale = glm::vec3(1.0f);
        }

        void SetTranslation(const glm::vec3& translation)
        {
            m_Translation = translation;
        }

        void SetRotation(const glm::quat& rotation)
        {
            m_Rotation = rotation;
        }

        void SetRotation(const glm::vec3& eulerAngles)
        {
            m_Rotation = glm::quat(eulerAngles);
        }

        void SetScale(const glm::vec3& scale)
        {
            m_Scale = scale;
        }

        glm::mat4 ToMatrix() const
        {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 rotation = glm::toMat4(m_Rotation);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);
            return translation * rotation * scale;
        }

        static Transform FromMatrix(const glm::mat4& matrix)
        {
            glm::vec3 translation, scale, skew;
            glm::vec4 perspective;
            glm::quat rotation;
            glm::decompose(matrix, scale, rotation, translation, skew, perspective);

            Transform t;
            t.m_Translation = translation;
            t.m_Scale = scale;
            t.m_Rotation = rotation;
            return t;
        }
    };
}