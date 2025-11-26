#include "EditorCamera.h"
#include "../Input/Input.h"

namespace Isle
{
    void EditorCamera::Start()
    {
        m_Camera = new Camera();
        SetName("EditorCamera");
        AddChild(m_Camera);

        m_Camera->SetFov(60.0f);
        m_Camera->SetNear(0.1f);
        m_Camera->SetFar(1000.0f);

        m_Target = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Distance = 17.32f;

        m_Camera->SetPosition(glm::vec3(10.0f, 10.0f, 10.0f));
        m_Camera->SetLook(m_Target);
    }

    void EditorCamera::Update(float delta_time)
    {
        if (!Input::Instance()->IsWindowFocused())
            return;

        glm::vec2 mouseDelta = Input::Instance()->GetMouseDelta();
        glm::vec2 scrollDelta = Input::Instance()->GetScrollDelta();

        bool shiftHeld = Input::Instance()->IsKeyHeld(GLFW_KEY_LEFT_SHIFT) ||
            Input::Instance()->IsKeyHeld(GLFW_KEY_RIGHT_SHIFT);

        if (scrollDelta.y != 0.0f)
        {
            float zoomSpeed = m_Distance * 0.1f;
            m_Distance -= scrollDelta.y * zoomSpeed;
            m_Distance = glm::clamp(m_Distance, 1.0f, 500.0f);

            glm::vec3 newPosition = m_Target - m_Camera->m_Front * m_Distance;
            m_Camera->SetPosition(newPosition);
        }

        if (shiftHeld && Input::Instance()->IsMouseButtonHeld(Input::MouseButton::Middle))
        {
            float panSpeed = m_Distance * 0.001f;
            glm::vec3 offset =
                m_Camera->m_Right * (-mouseDelta.x * panSpeed) +
                m_Camera->m_Up * (mouseDelta.y * panSpeed);

            m_Target += offset;
            m_Camera->Move(offset);
        }
        else if (Input::Instance()->IsMouseButtonHeld(Input::MouseButton::Middle))
        {
            float rotateSpeed = 0.2f;
            m_Camera->Rotate(-mouseDelta.y * rotateSpeed, mouseDelta.x * rotateSpeed);

            glm::vec3 newPosition = m_Target - m_Camera->m_Front * m_Distance;
            m_Camera->SetPosition(newPosition);
            m_Camera->SetLook(m_Target);
        }
    }

    void EditorCamera::Destroy()
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