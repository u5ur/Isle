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

        Scene::Instance()->Add(MainCamera::Instance());
        Scene::Instance()->Add(CameraMan::Instance());
        Scene::Instance()->Add(World::Instance());

        s_LastFrameTime = std::chrono::high_resolution_clock::now();

        const int cubeCount = 2;
        const float areaSize = 10.0f;

        {
            CubeMesh* cube = new CubeMesh();
            cube->SetColor(glm::vec4(1, 1, 1, 1));
            cube->SetLocalScale(glm::vec3(1.0f));
            World::Instance()->AddChild(cube);
        }

        {
            CubeMesh* cube = new CubeMesh();
            cube->SetColor(glm::vec4(1, 1, 1, 1));
            cube->SetLocalScale(glm::vec3(1.0f));
            cube->SetLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
            World::Instance()->AddChild(cube);
        }

        {
            CubeMesh* cube = new CubeMesh();
            cube->SetColor(glm::vec4(1, 1, 1, 1));
            cube->SetLocalScale(glm::vec3(10.0f, 0.1f, 10.0f));
            World::Instance()->AddChild(cube);
        }

        Scene::Instance()->Start();
    }

    void Application::Update()
    {
        auto now = std::chrono::high_resolution_clock::now();
        m_DeltaTime = std::chrono::duration<float>(now - s_LastFrameTime).count();
        s_LastFrameTime = now;

        Render::Instance()->BeginFrame();
        Input::Instance()->Update();

        Scene::Instance()->Update(m_DeltaTime);

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
