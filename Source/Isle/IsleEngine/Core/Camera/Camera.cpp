// Camera.cpp
#include "Camera.h"

namespace Isle
{
    Camera::Camera()
        : m_WorldUp(0.0f, 1.0f, 0.0f)
        , m_Front(0.0f, 0.0f, -1.0f)
        , m_Fov(90.0f)
        , m_AspectRatio(16.0f / 9.0f)
        , m_Near(0.1f)
        , m_Far(10000.0f)
        , m_Pitch(0.0f)
        , m_Yaw(0.0f)
    {
        SetName("Camera");
        UpdateVectors();
        UpdateViewMatrix();
        UpdateProjectionMatrix();
    }

    void Camera::SetLook(glm::vec3 target)
    {
        m_Front = glm::normalize(target - m_Transform.m_Translation);
        UpdateVectors();
        UpdateViewMatrix();
    }

    void Camera::SetFov(float fov)
    {
        m_Fov = fov;
        UpdateProjectionMatrix();
    }

    void Camera::SetAspectRatio(float ratio)
    {
        m_AspectRatio = ratio;
        UpdateProjectionMatrix();
    }

    void Camera::SetNear(float fnear)
    {
        m_Near = fnear;
        UpdateProjectionMatrix();
    }

    void Camera::SetFar(float ffar)
    {
        m_Far = ffar;
        UpdateProjectionMatrix();
    }

    void Camera::SetPosition(glm::vec3 position)
    {
        m_Transform.m_Translation = position;
        UpdateViewMatrix();
    }

    void Camera::SetRotation(glm::vec3 rotation)
    {
        m_Pitch = rotation.x;
        m_Yaw = rotation.y;

        if (m_Pitch > 89.0f) m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;

        UpdateVectors();
        UpdateViewMatrix();
    }

    void Camera::UpdateViewMatrix()
    {
        m_ViewMatrix = glm::lookAt(m_Transform.m_Translation, m_Transform.m_Translation + m_Front, m_Up);
    }

    void Camera::UpdateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_Near, m_Far);
    }

    void Camera::UpdateVectors()
    {
        float pitch = glm::radians(m_Pitch);
        float yaw = glm::radians(m_Yaw);

        glm::vec3 front;
        front.x = cos(yaw) * cos(pitch);
        front.y = sin(pitch);
        front.z = sin(yaw) * cos(pitch);

        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    void Camera::Move(glm::vec3 offset)
    {
        m_Transform.m_Translation += offset;
        UpdateViewMatrix();
    }

    void Camera::Rotate(float pitchDelta, float yawDelta)
    {
        m_Pitch += pitchDelta;
        m_Yaw += yawDelta;

        if (m_Pitch > 89.0f) m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;

        UpdateVectors();
        UpdateViewMatrix();
    }

    GpuCamera Camera::GetCpuCamera()
    {
        if (!this)
            return {};

        GpuCamera GCamera{};
        GCamera.m_CameraPos = m_Transform.m_Translation;
        GCamera.m_ProjectionMatrix = m_ProjectionMatrix;
        GCamera.m_ViewMatrix = m_ViewMatrix;
        return GCamera;
    }
}