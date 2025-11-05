// MainCamera.cpp
#include "MainCamera.h"

namespace Isle
{

    void MainCamera::Start()
    {
        this->SetName("MainCamera");
        AddChild(new Camera());
    }

    void MainCamera::Update(float delta_time)
    {

    }

    Camera* MainCamera::GetCamera()
    {
        return GetChild<Camera>();
    }

    void MainCamera::SetCamera(Camera* camera)
    {
        SetChild(0, camera);
    }
}
