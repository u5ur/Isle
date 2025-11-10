#include "Engine.h"
#include <Core/Scene/Scene.h>
#include <Core/Graphics/Render.h>

namespace Isle
{
    static std::chrono::high_resolution_clock::time_point s_LastFrameTime;

    void Engine::Start()
    {
        s_LastFrameTime = std::chrono::high_resolution_clock::now();
    }

    void Engine::Update()
    {
        auto now = std::chrono::high_resolution_clock::now();
        m_DeltaTime = std::chrono::duration<double>(now - s_LastFrameTime).count();
        m_FPS = 1.0f / m_DeltaTime;

        // everything happens in these two functions
        Scene::Instance()->Update(m_DeltaTime);
        Render::Instance()->RenderFrame();

        s_LastFrameTime = now;
    }

    void Engine::Destroy()
    {
        Scene::Instance()->ClearAll();
    }
}
