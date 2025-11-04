#include "CameraMan.h"
#include "../Input/Input.h"

namespace Isle
{
    void CameraMan::Start()
    {
        this->SetName("CameraMan");
        m_Camera = new Camera();
    }

    void CameraMan::Update(float delta_time)
    {
        const float baseMoveSpeed = 5.0f;
        const float sprintMultiplier = 2.0f;
        const float lookSpeed = 0.1f;

        if (!Input::Instance()->IsWindowFocused() || !Input::Instance()->IsCursorLocked())
            return;

        glm::vec2 mouseDelta = Input::Instance()->GetMouseDelta();
        m_Camera->m_Yaw += mouseDelta.x * lookSpeed;
        m_Camera->m_Pitch -= mouseDelta.y * lookSpeed;
        m_Camera->m_Pitch = glm::clamp(m_Camera->m_Pitch, -89.0f, 89.0f);

        m_Camera->Rotate(0.0f, 0.0f);
        float speed = baseMoveSpeed;
        if (Input::Instance()->IsActionHeld("Sprint"))
            speed *= sprintMultiplier;

        glm::vec3 velocity(0.0f);

        if (Input::Instance()->IsActionHeld("Forward"))
            velocity += m_Camera->m_Front;
        if (Input::Instance()->IsActionHeld("Backward"))
            velocity -= m_Camera->m_Front;
        if (Input::Instance()->IsActionHeld("Right"))
            velocity += m_Camera->m_Right;
        if (Input::Instance()->IsActionHeld("Left"))
            velocity -= m_Camera->m_Right;
        if (Input::Instance()->IsActionHeld("Jump"))
            velocity += m_Camera->m_Up;

        if (glm::length(velocity) > 0.0f)
            velocity = glm::normalize(velocity);

        m_Camera->Move(velocity * speed * delta_time);
    }
}