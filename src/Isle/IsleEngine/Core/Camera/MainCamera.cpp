#include "MainCamera.h"
#include "CameraMan.h"
#include "EditorCamera.h"
#include "OrthographicCamera.h"

namespace Isle
{
    void MainCamera::Start()
    {
        SetName("MainCamera");
    }

    void MainCamera::Update(float delta_time)
    {
    }

    Camera* MainCamera::GetCamera()
    {
        return m_CurrentCamera;
    }

    void MainCamera::SetCamera(Camera* camera)
    {
        m_Children.clear();
        m_CurrentCamera = camera;

        if (camera)
        {
            camera->m_Owner = this;
            m_Children.push_back(camera);
        }
    }
}
