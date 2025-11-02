#include "IsleGame.h"
#include <iostream>

namespace Isle
{
    void Application::Start()
    {
        ISLE_LOG("Application Started!\n");
    }

    void Application::Update()
    {

    }

    void Application::Destroy()
    {
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
