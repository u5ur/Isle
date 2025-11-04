// MainCamera.cpp
#include "MainCamera.h"
#include "CameraMan.h"

namespace Isle
{
    void MainCamera::SetCamera(Camera* camera)
    {
        m_Camera = camera;
    }

    void MainCamera::Start()
    {
        this->SetName("MainCamera");
        m_Camera = new Camera();
    }

    void MainCamera::Update(float delta_time)
    {
        m_Camera = CameraMan::Instance()->m_Camera;
    }
}
