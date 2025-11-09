// MainCamera.cpp
#include "MainCamera.h"
#include "CameraMan.h"
#include "EditorCamera.h"
#include "OrthographicCamera.h"

namespace Isle
{

    void MainCamera::Start()
    {
        this->SetName("MainCamera");
        AddChild(new Camera());
    }

    void MainCamera::Update(float delta_time)
    {
        if (m_Children.size() < 1)
            m_Children.resize(1);

        if (m_Type == CAMERA_TYPE::DEFAULT_PERSPECTIVE)
        {
            m_Children[0] = CameraMan::Instance()->m_Camera;
        }
        else if(m_Type == CAMERA_TYPE::EDITOR_PERSPECTIVE)
        {
            m_Children[0] = EditorCamera::Instance()->m_Camera;
        }
        else if (m_Type == CAMERA_TYPE::ORTHOGRAPHIC)
        {
            m_Children[0] = OrthographicCamera::Instance()->m_Camera;
        }
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
