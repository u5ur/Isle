#include "CameraMan.h"
#include "../Input/Input.h"
#include <Core/Camera/MainCamera.h>

namespace Isle
{
    void CameraMan::Start()
    {
        this->SetName("CameraMan");
    }

    void CameraMan::Update(float delta_time)
    {
        const float baseMoveSpeed = 5.0f;
        const float sprintMultiplier = 2.0f;
        const float lookSpeed = 0.1f;

        if (!Input::Instance()->IsWindowFocused() || !Input::Instance()->IsCursorLocked())
            return;

        auto Camera = MainCamera::Instance()->GetCamera();
        if (!Camera)
        {
            ISLE_ERROR("Failed to Camera from CameraMan!\n");
            return;
        }

        glm::vec2 mouseDelta = Input::Instance()->GetMouseDelta();
        Camera->m_Yaw += mouseDelta.x * lookSpeed;
        Camera->m_Pitch -= mouseDelta.y * lookSpeed;
        Camera->m_Pitch = glm::clamp(Camera->m_Pitch, -89.0f, 89.0f);

        Camera->Rotate(0.0f, 0.0f);
        float speed = baseMoveSpeed;
        if (Input::Instance()->IsActionHeld("Sprint"))
            speed *= sprintMultiplier;

        glm::vec3 velocity(0.0f);

        if (Input::Instance()->IsActionHeld("Forward"))
            velocity += Camera->m_Front;
        if (Input::Instance()->IsActionHeld("Backward"))
            velocity -= Camera->m_Front;
        if (Input::Instance()->IsActionHeld("Right"))
            velocity += Camera->m_Right;
        if (Input::Instance()->IsActionHeld("Left"))
            velocity -= Camera->m_Right;
        if (Input::Instance()->IsActionHeld("Jump"))
            velocity += Camera->m_Up;

        if (glm::length(velocity) > 0.0f)
            velocity = glm::normalize(velocity);

        Camera->Move(velocity * speed * delta_time);
    }
}