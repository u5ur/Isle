// MainCamera.cpp
#include "MainCamera.h"

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

    }
}
