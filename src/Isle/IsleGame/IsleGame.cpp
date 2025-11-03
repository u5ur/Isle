#include "IsleGame.h"
#include <iostream>

namespace Isle
{
    void Application::Start()
    {
        m_Window = new Window();
        m_Window->Start();
        Render::Instance()->Start();

        ISLE_LOG("Application Started!\n");
    }

    void Application::Update()
    {
        m_Window->Update();
        Render::Instance()->Update();
    }

    void Application::Destroy()
    {
        m_Window->Destroy();
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
