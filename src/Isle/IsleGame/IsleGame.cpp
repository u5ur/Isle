#include "IsleGame.h"
#include <chrono>

namespace Isle
{
    static std::chrono::high_resolution_clock::time_point s_LastFrameTime;

    void Application::Start()
    {
        Scene::Instance()->ClearAll();
        CameraMan::Instance()->Start();
        EditorCamera::Instance()->Start();
        OrthographicCamera::Instance()->Start();
        MainCamera::Instance()->Start();
        World::Instance()->Start();

        Scene::Instance()->Add(MainCamera::Instance(), false);
        Scene::Instance()->Add(CameraMan::Instance(), false);
        Scene::Instance()->Add(OrthographicCamera::Instance(), false);
        Scene::Instance()->Add(EditorCamera::Instance(), false);
        Scene::Instance()->Add(World::Instance(), false);

        auto Asset = AssetManager::Instance()->Load(
            "C:\\Users\\asdf\\Documents\\IsleEngineOpenGl\\source\\assets\\models\\Sponza\\scene.gltf");
        Scene::Instance()->Add(dynamic_cast<SceneComponent*>(Asset->m_RootObject), true);

        s_LastFrameTime = std::chrono::high_resolution_clock::now();

        Scene::Instance()->Start();
    }

    void Application::Update()
    {
        auto now = std::chrono::high_resolution_clock::now();

        if (m_Paused)
        {
            m_DeltaTime = 0.0f;
        }
        else
        {
            m_DeltaTime = std::chrono::duration<float>(now - s_LastFrameTime).count();
        }
        s_LastFrameTime = now;

        if (!m_IsEditorMode)
        {
            Render::Instance()->BeginFrame();
            Scene::Instance()->Update(m_DeltaTime);
            Render::Instance()->RenderFrame();
            Render::Instance()->EndFrame();
        }
        else
        {
            Scene::Instance()->Update(m_DeltaTime);
            Render::Instance()->RenderFrame();
        }
    }

    void Application::Destroy()
    {
        Scene::Instance()->ClearAll();
    }
}

void* CreateApplication()
{
    return new Isle::Application();
}

void DestroyApplication(void* app)
{
    if (app)
    {
        delete static_cast<Isle::Application*>(app);
    }
}

void Application_Start(void* app)
{
    if (app)
    {
        static_cast<Isle::Application*>(app)->Start();
    }
}

void Application_Update(void* app)
{
    if (app)
    {
        static_cast<Isle::Application*>(app)->Update();
    }
}

void Application_Destroy(void* app)
{
    if (app)
    {
        static_cast<Isle::Application*>(app)->Destroy();
    }
}

void Application_SetEditorMode(void* app, bool mode)
{
    if (app)
    {
        static_cast<Isle::Application*>(app)->m_IsEditorMode = mode;
    }
}

void Application_SetPaused(void* app, bool paused)
{
    if (app)
    {
        static_cast<Isle::Application*>(app)->m_Paused = paused;
    }
}
