// OrthographicCamera.cpp 
#include "OrthographicCamera.h" 
#include "../Input/Input.h" 
#include <Core/Camera/MainCamera.h> 

namespace Isle
{
    void OrthographicCamera::Start()
    {
        m_Camera = new Camera();
        SetName("OrthographicCamera");
        AddChild(m_Camera);

        m_ZoomLevel = 10.0f;
        m_Left = -10.0f;
        m_Right = 10.0f;
        m_Bottom = -10.0f;
        m_Top = 10.0f;

        m_Target = glm::vec3(0.0f);
        m_Distance = 50.0f;
        m_Pitch = 30.0f;
        m_Yaw = -45.0f;

        UpdateCameraPosition();
    }

    void OrthographicCamera::Update(float delta_time)
    {
        if (!Input::Instance()->IsWindowFocused())
            return;

        glm::vec2 mouseDelta = Input::Instance()->GetMouseDelta();
        bool ctrlHeld = Input::Instance()->IsKeyHeld(GLFW_KEY_LEFT_CONTROL) ||
            Input::Instance()->IsKeyHeld(GLFW_KEY_RIGHT_CONTROL);

        if (ctrlHeld && Input::Instance()->IsMouseButtonHeld(Input::MouseButton::Left))
        {
            m_ZoomLevel += mouseDelta.y * 0.05f;
            m_ZoomLevel = glm::clamp(m_ZoomLevel, 0.05f, 5.0f);
        }
        else if (Input::Instance()->IsMouseButtonHeld(Input::MouseButton::Right))
        {
            glm::vec3 panOffset =
                m_Camera->m_Right * (-mouseDelta.x * 0.01f * m_ZoomLevel) +
                m_Camera->m_Up * (mouseDelta.y * 0.01f * m_ZoomLevel);

            m_Target += panOffset;
            UpdateCameraPosition();
        }
        else if (Input::Instance()->IsMouseButtonHeld(Input::MouseButton::Middle))
        {
            m_Yaw += mouseDelta.x * 0.2f;
            m_Pitch -= mouseDelta.y * 0.2f;
            m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
            UpdateCameraPosition();
        }

        m_Camera->m_ProjectionMatrix = glm::ortho(
            m_Left * m_ZoomLevel,
            m_Right * m_ZoomLevel,
            m_Bottom * m_ZoomLevel,
            m_Top * m_ZoomLevel,
            m_Camera->m_Near,
            m_Camera->m_Far
        );
    }

    void OrthographicCamera::UpdateCameraPosition()
    {
        float pitchRad = glm::radians(m_Pitch);
        float yawRad = glm::radians(m_Yaw);

        glm::vec3 direction(
            cos(yawRad) * cos(pitchRad),
            sin(pitchRad),
            sin(yawRad) * cos(pitchRad)
        );
        direction = glm::normalize(direction);

        glm::vec3 position = m_Target - direction * m_Distance;
        m_Camera->SetPosition(position);

        m_Camera->m_Front = direction;
        m_Camera->m_Right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_Camera->m_Up = glm::normalize(glm::cross(m_Camera->m_Right, direction));
        m_Camera->m_ViewMatrix = glm::lookAt(position, m_Target, m_Camera->m_Up);
    }

    void OrthographicCamera::SetOrthographicBounds(float left, float right, float bottom, float top)
    {
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;
    }

    void OrthographicCamera::SetZoomLevel(float zoom)
    {
        m_ZoomLevel = glm::clamp(zoom, 0.5f, 50.0f);
    }

    void OrthographicCamera::SetTarget(glm::vec3 target)
    {
        m_Target = target;
        UpdateCameraPosition();
    }

    void OrthographicCamera::Destroy()
    {
        if (m_Camera)
        {
            m_Camera->Destroy();
            delete m_Camera;
            m_Camera = nullptr;
        }

        SceneComponent::Destroy();
    }
}