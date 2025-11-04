// IsleGame.cpp
#include "IsleGame.h"
#include <iostream>
#include <chrono>

namespace Isle
{
    PrimitiveMesh* Cube_Mesh = nullptr;

    static std::chrono::high_resolution_clock::time_point s_LastFrameTime;

    void Application::Start()
    {
        Render::Instance()->Start();
        Input::Instance()->Start();
        MainCamera::Instance()->Start();
        CameraMan::Instance()->Start();

        s_LastFrameTime = std::chrono::high_resolution_clock::now();

        CameraMan::Instance()->m_Camera->SetPosition(glm::vec3(0, 20, 40));
        CameraMan::Instance()->m_Camera->SetLook(glm::vec3(0, 0, 0));

        const int cubeCount = 300;
        const float areaSize = 50.0f;

        for (int i = 0; i < cubeCount; ++i)
        {
            CubeMesh* cube = new CubeMesh();
            cube->SetColor(glm::vec4(1, 1, 1, 1));

            float x = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * areaSize;
            float y = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * areaSize * 0.2f;
            float z = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * areaSize;

            cube->SetLocalPosition(glm::vec3(x, y, z));

            float s = 0.5f + ((float)rand() / RAND_MAX) * 1.5f;
            cube->SetLocalScale(glm::vec3(s));

            Render::Instance()->GetPipeline()->AddStaticMesh(cube);
        }
    }

    void Application::Update()
    {
        auto now = std::chrono::high_resolution_clock::now();
        m_DeltaTime = std::chrono::duration<float>(now - s_LastFrameTime).count();
        s_LastFrameTime = now;

        Render::Instance()->BeginFrame();

        Input::Instance()->Update();
        CameraMan::Instance()->Update(m_DeltaTime);
        MainCamera::Instance()->Update(m_DeltaTime);
        Render::Instance()->GetPipeline()->SetCamera(MainCamera::Instance()->m_Camera);

        Render::Instance()->RenderFrame();
        Render::Instance()->EndFrame();
    }

    void Application::Destroy()
    {
        Render::Instance()->Destroy();
        ISLE_LOG("Application Destroyed!\n");
    }

    Application* Application::Create()
    {
        return new Application();
    }
}

Isle::Application* CreateApplication()
{
    return Isle::Application::Create();
}
